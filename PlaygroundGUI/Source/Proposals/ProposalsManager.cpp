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
#include "../Data/AutomatonContractData.h"
#include "automaton/core/interop/ethereum/eth_contract_curl.h"
#include "automaton/core/interop/ethereum/eth_transaction.h"
#include "automaton/core/interop/ethereum/eth_helper_functions.h"
#include "automaton/core/common/status.h"

using automaton::core::common::status;
using automaton::core::interop::ethereum::dec_to_32hex;
using automaton::core::interop::ethereum::eth_transaction;
using automaton::core::interop::ethereum::eth_getTransactionCount;
using automaton::core::interop::ethereum::eth_getTransactionReceipt;
using automaton::core::interop::ethereum::encode;
using automaton::core::interop::ethereum::eth_contract;
using automaton::core::io::bin2hex;
using automaton::core::io::dec2hex;
using automaton::core::io::hex2dec;

class AsyncTask : public ThreadWithProgressWindow {
public:
  AsyncTask (std::function<bool(AsyncTask*)> fun, const String& title) :
    ThreadWithProgressWindow (title, true, true),
    m_status (status::ok()) {
    m_fun = fun;
  }

  void run() {
    m_fun (this);
  }

  status m_status;

private:
  std::function<bool(AsyncTask*)> m_fun;
};

ProposalsManager::ProposalsManager()
  : m_model (std::make_shared<ProposalsModel>())
{
}

ProposalsManager::~ProposalsManager()
{
  clearSingletonInstance();
}

bool ProposalsManager::fetchProposals()
{
  const auto cd = AutomatonContractData::getInstance();
  const auto contract = eth_contract::get_contract (cd->contract_address);
  if (contract == nullptr) {
    std::cout << "ERROR: Contract is NULL" << std::endl;
    AlertWindow::showMessageBoxAsync (AlertWindow::WarningIcon,
                                      "ERROR",
                                      "Contract is NULL. Read appropriate contract data first.");
    //s = status::internal("Contract is NULL!");
    return false;
  }

  static const int PROPOSAL_START_ID = 100; // ballotBoxIDs initial value is 99, and the first proposal is at 100
  auto s = contract->call ("ballotBoxIDs", "");
  if (!s.is_ok())
  {
      std::cout << "ERROR: " << s.msg << std::endl;
      return false;
  }
  if (s.msg.empty())
  {
      s = status::internal ("Invalid contract address!");
      std::cout << "ERROR: Invalid contract address" << std::endl;
      return false;
  }

  // Clear all existing proposals in the model
  m_model->clear (false);

  const json ballotBoxJson = json::parse (s.msg);
  const uint32_t lastProposalId = std::stoul ((*ballotBoxJson.begin()).get<std::string>());
  for (int i = PROPOSAL_START_ID; i <= lastProposalId; ++i)
  {
    json jInput;
    jInput.push_back (i);
    std::string params = jInput.dump();
    // Enable in new contract version
    //auto s = contract->call ("getProposal", params);
    auto s = contract->call ("proposals", params);

    if (! s.is_ok())
    {
      std::cout << "ERROR: " << s.msg << std::endl;
      return false;
    }

    Proposal proposal (i, String (s.msg));
    addProposal (proposal, false);
  }

  return true;
}

bool ProposalsManager::addProposal (const Proposal& proposal, bool sendNotification)
{
  m_model->addItem (std::make_shared<Proposal> (proposal), sendNotification);
}

bool ProposalsManager::createProposal (Proposal::Ptr proposal, const std::string& contributor)
{
  AsyncTask task ([=](AsyncTask* task)
  {
    //TODO: pass address and privateKey
    const std::string privateKey = "d6bcab62129f74fc2dcf9f1016625c054765f2743a24ee2326b9790693051283";
    const std::string address = "0x266f746a6d9b01085Cf2c20ba55ECC45A4D51724";

    const auto cd = AutomatonContractData::getInstance();
    const auto contract = eth_contract::get_contract (cd->contract_address);
    if (contract == nullptr)
    {
      std::cout << "ERROR: Contract is NULL" << std::endl;
      AlertWindow::showMessageBoxAsync (AlertWindow::WarningIcon,
              "ERROR",
              "Contract is NULL. Read appropriate contract data first.");
      //s = status::internal("Contract is NULL!");
      return false;
    }
    task->setProgress (0.1);

    auto s = contract->call ("ballotBoxIDs", "");
    if (! s.is_ok())
    {
      std::cout << "ERROR: " << s.msg << std::endl;
      return false;
    }
    const json ballotBoxJson = json::parse (s.msg);
    const uint32_t lastProposalId = std::stoul ((*ballotBoxJson.begin()).get<std::string>());
    proposal->setId (lastProposalId + 1);
    task->setProgress (0.25);

    s = eth_getTransactionCount (cd->eth_url, address);
    const auto nonce = s.is_ok() ? s.msg : "0";
    if (! s.is_ok())
      return false;

    json jProposal;
    jProposal.push_back (contributor);
    jProposal.push_back (proposal->getTitle().toStdString());
    jProposal.push_back ("google.com");
    jProposal.push_back ("BA5EC0DE");
    jProposal.push_back (proposal->getLengthDays());
    jProposal.push_back (proposal->getNumPeriods());
    jProposal.push_back (proposal->getBudget());

    json jSignature;
    jSignature.push_back ("address");
    jSignature.push_back ("string");
    jSignature.push_back ("string");
    jSignature.push_back ("bytes");
    jSignature.push_back ("uint256");
    jSignature.push_back ("uint256");
    jSignature.push_back ("uint256");

    std::stringstream createProposalData;
    createProposalData << "f8a1ff12" << bin2hex (encode (jSignature.dump(), jProposal.dump()));
    task->setProgress (0.5);

    eth_transaction transaction;
    transaction.nonce = nonce;
    transaction.gas_price = "1388";  // 5 000
    transaction.gas_limit = "5B8D80";  // 6M
    transaction.to = cd->contract_address.substr(2);
    transaction.value = "";
    transaction.data = createProposalData.str();
    transaction.chain_id = "01";
    s = contract->call ("createProposal", transaction.sign_tx (privateKey));

    if (! s.is_ok())
      return false;

    DBG("Call result: " << s.msg << "\n");
    proposal->setStatus (Proposal::Status::Started);
    task->setProgress (1.0);

    return true;
  }, "Creating proposal....");


  if (task.runThread())
  {
    m_model->addItem (proposal);
  }
  else
  {
    auto& s = task.m_status;
    if (! s.is_ok())
    {
      printf("ERROR: (%u) %s", s.code, s.msg.c_str());
      AlertWindow::showMessageBoxAsync (AlertWindow::WarningIcon,
              "ERROR",
              String("(") + String(s.code) + String(") :") + s.msg);
      return false;
    }
  }

  return true;
}

void ProposalsManager::notifyProposalsUpdated()
{
  m_model->notifyModelChanged();
}

JUCE_IMPLEMENT_SINGLETON(ProposalsManager);
