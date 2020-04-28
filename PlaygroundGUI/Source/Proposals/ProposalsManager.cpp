/*
 * Automaton Playground
 * Copyright (c) 2020 The Automaton Authors.
 * Copyright (c) 2020 The automaton.network Authors.
 *
 * Automaton Playground is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * Automaton Playground is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Automaton Playground.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <json.hpp>
#include <functional>

#include "ProposalsManager.h"
#include "Utils/AsyncTask.h"
#include "Utils/TasksManager.h"
#include "Data/AutomatonContractData.h"

#include "automaton/core/interop/ethereum/eth_contract_curl.h"
#include "automaton/core/interop/ethereum/eth_transaction.h"
#include "automaton/core/interop/ethereum/eth_helper_functions.h"
#include "automaton/core/common/status.h"

using automaton::core::common::status;
using automaton::core::interop::ethereum::eth_contract;
using automaton::core::io::bin2hex;
using automaton::core::io::dec2hex;
using automaton::core::io::hex2dec;

static uint64 getNumSlots(AutomatonContractData::Ptr contract, status* resStatus);
static uint64 getNumSlotsPaid(AutomatonContractData::Ptr contract, uint64 proposalId, status* resStatus);
static std::vector<std::string> getOwners(AutomatonContractData::Ptr contract, uint64 numOfSlots, status* resStatus);
static uint64 getLastProposalId(AutomatonContractData::Ptr contract, status* resStatus);
static void voteWithSlot(AutomatonContractData::Ptr contract,
                         uint64 id, uint64 slot, uint64 choice,
                         const std::string& privateKey,
                         status* resStatus);

ProposalsManager::ProposalsManager(Account::Ptr accountData)
  : m_model(std::make_shared<ProposalsModel>())
  , m_accountData(accountData) {
  m_contractData = m_accountData->getContractData();
}

ProposalsManager::~ProposalsManager() {
}

bool ProposalsManager::fetchProposals() {
  TasksManager::launchTask([&](AsyncTask* task) {
    auto& s = task->m_status;

    m_model->clear(NotificationType::dontSendNotification);

    const auto lastProposalId = getLastProposalId(m_contractData, &s);
    if (!s.is_ok())
      return false;

    task->setStatusMessage("Fetching proposals...");

    // ballotBoxIDs initial value is 99, and the first proposal is at 100
    static const uint32_t PROPOSAL_START_ID = 100;
    Array<Proposal::Ptr> proposals;

    for (int i = PROPOSAL_START_ID; i <= lastProposalId; ++i) {
      json jInput;
      jInput.push_back(i);
      std::string params = jInput.dump();

      s = m_contractData->call("getProposalInfo", params);

      if (!s.is_ok())
        return false;

      const String proposalInfoJson = s.msg;

      s = m_contractData->call("getProposalData", params);

      if (!s.is_ok())
        return false;

      const String proposalDataJson = s.msg;
      auto proposal = std::make_shared<Proposal>(i, proposalInfoJson, proposalDataJson);

      s = m_contractData->call("calcVoteDifference", params);
      if (!s.is_ok())
        return false;

      json j_output = json::parse(s.msg);
      const int approvalRating = std::stoi((*j_output.begin()).get<std::string>());
      proposal->setApprovalRating(approvalRating);

      const auto numSlotsPaid = getNumSlotsPaid(m_contractData, i, &s);
      if (!s.is_ok())
        return false;

      proposal->setNumSlotsPaid(numSlotsPaid);
      const bool areAllSlotsPaid = (numSlotsPaid == m_accountData->getContractData()->getSlotsNumber());
      proposal->setAllSlotsPaid(areAllSlotsPaid);
      if (!areAllSlotsPaid)
        proposal->setStatus(Proposal::Status::PrepayingGas);

      // TODO(Kirill): set all aliases for all accounts we have
      if (String(getEthAddress()).substring(2).equalsIgnoreCase(proposal->getCreator()))
        proposal->setCreatorAlias(getEthAddressAlias());

      proposals.add(proposal);
    }

    m_model->addItems(proposals, NotificationType::sendNotificationAsync);
    task->setStatusMessage("Fetched " + String(m_model->size()) + " proposals");

    return true;
  }, [=](AsyncTask* task) {
  }, "Fetching proposals..."
   , m_accountData);

  return true;
}

bool ProposalsManager::fetchProposalVotes(Proposal::Ptr proposal) {
    const auto topicName = proposal->getTitle() + " (" + String(proposal->getId()) + ") " + "Fetch votes";
    TasksManager::launchTask([&, proposal](AsyncTask* task) {
    auto& s = task->m_status;
    const int numOfSlots = m_accountData->getContractData()->getSlotsNumber();
    task->setStatusMessage("Fetching " + String(numOfSlots) + " votes for proposal "
                           + proposal->getTitle() + " (" + String(proposal->getId()) + ")");

    Array<uint64> slots;
    for (int i = 0; i < numOfSlots; ++i) {
      json jInput;
      jInput.push_back(proposal->getId());
      jInput.push_back(i);
      std::string params = jInput.dump();

      s = m_contractData->call("getVote", params);

      if (!s.is_ok())
        return false;

      json j_output = json::parse(s.msg);
      uint64 slotVote = static_cast<uint64>(String((*j_output.begin()).get<std::string>()).getLargeIntValue());
      slots.add(slotVote);
      task->setProgress(i / static_cast<double>(numOfSlots));
    }

    proposal->setSlots(slots, NotificationType::sendNotification);
    task->setStatusMessage("Fetched " + String(numOfSlots) + " votes for proposal "
                           + proposal->getTitle() + " (" + String(proposal->getId()) + ")");

    return true;
  }, [=](AsyncTask* task) {
  }, topicName, m_accountData);

  return true;
}

void ProposalsManager::addProposal(Proposal::Ptr proposal, NotificationType notification) {
  m_model->addItem(proposal, notification);
}

bool ProposalsManager::createProposal(Proposal::Ptr proposal, const String& contributor) {
  const auto topicName = proposal->getTitle() + " (" + String(proposal->getId()) + ") " + "Create proposal";
  TasksManager::launchTask([=](AsyncTask* task) {
    auto& s = task->m_status;

    task->setProgress(0.1);

    const auto lastProposalId = getLastProposalId(m_contractData, &s);
    proposal->setId(lastProposalId + 1);
    task->setProgress(0.25);

    const auto contributor_address = contributor.startsWith("0x")
                                        ? contributor.substring(2).toStdString()
                                        : contributor.toStdString();
    json jProposal;
    jProposal.push_back(contributor_address);
    jProposal.push_back(proposal->getTitle().toStdString());
    jProposal.push_back("google.com");
    jProposal.push_back("BA5EC0DE");
    jProposal.push_back(proposal->getBudgetPeriodLength());
    jProposal.push_back(proposal->getNumPeriodsLeft());
    jProposal.push_back(proposal->getBudgetPerPeriod().toStdString());

    task->setProgress(0.5);

    s = m_contractData->call("createProposal", jProposal.dump(), m_accountData->getPrivateKey());
    if (!s.is_ok())
      return false;

    DBG("Call result: " << s.msg << "\n");
    proposal->setStatus(Proposal::Status::Started);
    task->setProgress(1.0);

    task->setStatusMessage("Proposal \"" + proposal->getTitle() + "\" successfully created");

    return true;
  }, [=](AsyncTask* task) {
  }, topicName, m_accountData);

  return true;
}

bool ProposalsManager::payForGas(Proposal::Ptr proposal, uint64 slotsToPay) {
  if (proposal->getId() <= 0) {
    AlertWindow::showMessageBoxAsync(
      AlertWindow::WarningIcon,
      "ERROR",
      "The proposal is not valid");
  }

  const auto topicName = "(" + String(proposal->getId()) + ") " + "Pay for gas";
  TasksManager::launchTask([=](AsyncTask* task) {
    auto& s = task->m_status;

    task->setProgress(0.1);

    json jInput;
    jInput.push_back(proposal->getId());
    jInput.push_back(slotsToPay);

    task->setProgress(0.5);
    task->setStatusMessage("Pay gas for " + String(slotsToPay) + " slots");
    s = m_contractData->call("payForGas", jInput.dump(), m_accountData->getPrivateKey());

    if (!s.is_ok())
      return false;

    task->setStatusMessage("Successfully paid gas for " + String(slotsToPay) + " slots");
    task->setProgress(1.0);

    return true;
  }, [=](AsyncTask* task) {
  }, topicName, m_accountData);

  return true;
}

void ProposalsManager::notifyProposalsUpdated() {
  m_model->notifyModelChanged(NotificationType::sendNotificationAsync);
}

static void voteWithSlot(AutomatonContractData::Ptr contract,
                         uint64 id, uint64 slot, uint64 choice,
                         const std::string& privateKey,
                         status* resStatus) {
  json jInput;
  jInput.push_back(id);
  jInput.push_back(slot);
  jInput.push_back(choice);

  *resStatus = contract->call("castVote",  jInput.dump(), privateKey);
}

static uint64 getNumSlots(AutomatonContractData::Ptr contract, status* resStatus) {
  auto s = contract->call("numSlots", "");
  *resStatus = s;
  if (!s.is_ok()) {
    std::cout << "ERROR: " << s.msg << std::endl;
    return 0;
  }

  json j_output = json::parse(s.msg);
  const uint64 slots_number = String((*j_output.begin()).get<std::string>()).getLargeIntValue();
  return slots_number;
}

static uint64 getNumSlotsPaid(AutomatonContractData::Ptr contract, uint64 proposalId, status* resStatus) {
  json j_input;
  j_input.push_back(proposalId);
  const std::string params = j_input.dump();

  // Fetch ballot box info for the given proposal
  const auto s = contract->call("getBallotBox", params);
  *resStatus = s;
  if (!s.is_ok())
    return 0;

  const json ballotBoxJson = json::parse(s.msg);
  if (ballotBoxJson.size() != 3) {
    return 0;
  }

  const uint64 numSlotsPaid = String(ballotBoxJson[2].get<std::string>()).getLargeIntValue();
  return numSlotsPaid;
}

static std::vector<std::string> getOwners(AutomatonContractData::Ptr contract,
                                          uint64 numOfSlots,
                                          status* resStatus) {
  json j_input;
  j_input.push_back(0);
  j_input.push_back(numOfSlots);
  const std::string params = j_input.dump();

  // Fetch owners.
  const auto s = contract->call("getOwners", params);
  *resStatus = s;
  if (!s.is_ok())
    return std::vector<std::string>();

  // Parse owners.
  const auto j_output = json::parse(s.msg);
  std::vector<std::string> owners = (*j_output.begin()).get<std::vector<std::string>>();
  return owners;
}

static uint64 getLastProposalId(AutomatonContractData::Ptr contract, status* resStatus) {
  auto s = contract->call("proposalsData", "");
  *resStatus = s;
  if (!s.is_ok())
    return 0;

  const json proposalsDataJson = json::parse(s.msg);
  if (proposalsDataJson.size() > 3) {
    const uint64 lastProposalId = String(proposalsDataJson[3].get<std::string>()).getLargeIntValue();
    return lastProposalId;
  }

  return 0;
}

bool ProposalsManager::castVote(Proposal::Ptr proposal, uint64 choice) {
  if (proposal->getId() <= 0) {
    AlertWindow::showMessageBoxAsync(
      AlertWindow::WarningIcon,
      "ERROR",
      "The proposal is not valid");
  }

  // TODO(Kirill) fetch choices names
  const auto choiceName = choice == 1 ? "YES" : choice == 2 ? "NO" : "Unspecified";
  const auto topicName = "(" + String(proposal->getId()) + ") " + "Vote " + choiceName;
  TasksManager::launchTask([=](AsyncTask* task) {
    auto& s = task->m_status;

    task->setProgress(0.01);

    const auto numOfSlots = getNumSlots(m_contractData, &s);

    if (!s.is_ok())
      return false;

    const auto owners = getOwners(m_contractData, numOfSlots, &s);

    if (!s.is_ok())
      return false;

    const String callAddress = m_accountData->getAddress().substr(2);

    uint64 numOwnedSlots = 0;
    bool isOwnerForAnySlot = false;
    for (uint64 slot = 0; slot < owners.size(); ++slot) {
      String owner = owners[slot];
      if (owner.equalsIgnoreCase(callAddress)) {
        isOwnerForAnySlot = true;
        ++numOwnedSlots;

        task->setStatusMessage("Voting for slot " + String(slot) + "...");
        voteWithSlot(m_contractData, proposal->getId(), slot,
            choice, m_accountData->getPrivateKey(), &s);

        if (!s.is_ok() || task->threadShouldExit())
          return false;
      }

      task->setProgress(slot / static_cast<double>(numOfSlots));
    }

    if (!isOwnerForAnySlot) {
      s = status::internal("You own no single slot. Voting is impossible");
      return false;
    }
    task->setStatusMessage("Successfully voted for " + String(numOwnedSlots) + " slots!");

    return true;
  }, [=](AsyncTask* task) {
  }, topicName, m_accountData);

  return true;
}

bool ProposalsManager::claimReward(Proposal::Ptr proposal, const String& rewardAmount) {
  if (proposal->getId() <= 0) {
    AlertWindow::showMessageBoxAsync(
      AlertWindow::WarningIcon,
      "ERROR",
      "The proposal is not valid");
  }
  const auto topicName = "(" + String(proposal->getId()) + ") " + "Claim reward";

  TasksManager::launchTask([=](AsyncTask* task) {
    auto& s = task->m_status;

    task->setProgress(0.1);
    task->setStatusMessage("Claiming reward...");

    json jInput;
    jInput.push_back(proposal->getId());
    jInput.push_back(rewardAmount.toStdString());

    s = m_contractData->call("claimReward", jInput.dump(), m_accountData->getPrivateKey());
    DBG("Call result: " << s.msg << "\n");

    if (!s.is_ok())
      return false;

    task->setProgress(1.0);
    task->setStatusMessage("Claiming reward...success!");

    return true;
  }, [=](AsyncTask* task) {
  }, topicName, m_accountData);

  return true;
}
