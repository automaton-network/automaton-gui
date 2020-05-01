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

#include "Proposal.h"

#include <json.hpp>
#include <string>

using json = nlohmann::json;


Proposal::Proposal()
  : m_id(0)
  , m_numPeriodsLeft(0)
  , m_budgetPeriodLength(0)
  , m_initialPeriod(0)
  , m_contestPeriod(0)
  , m_budgetPerPeriod(String(0))
  , m_amountSpent(String(0))
  , m_targetBonus(String(0))
  , m_approvalRating(0)
  , m_timeLeft(0)
  , m_numSlotsPaid(0)
  , m_areAllSlotsPaid(false)
  , m_status(Proposal::Status::Uninitialized) {
}

Proposal::Proposal(uint32_t id, const String& infoJsonString, const String& dataJsonString)
  : m_id(0)
  , m_numPeriodsLeft(0)
  , m_budgetPeriodLength(0)
  , m_initialPeriod(0)
  , m_contestPeriod(0)
  , m_budgetPerPeriod(String(0))
  , m_amountSpent(String(0))
  , m_targetBonus(String(0))
  , m_approvalRating(0)
  , m_timeLeft(0)
  , m_numSlotsPaid(0)
  , m_areAllSlotsPaid(false)
  , m_status(Proposal::Status::Uninitialized) {
    setId(id);

    json json_proposal_info = json::parse(infoJsonString.toStdString());
    json json_proposal_data = json::parse(dataJsonString.toStdString());
    // TODO(Kirill): add json error handling

    // Set "info" part
    setCreator(json_proposal_info.at(0).get<std::string>());
    setTitle(json_proposal_info.at(1).get<std::string>());
    setDocumentLink(json_proposal_info.at(2).get<std::string>());
    setDocumentHash(json_proposal_info.at(3).get<std::string>());
    // divide by timeUnitInSeconds
    setBudgetPeriodLength(std::stoul(json_proposal_info.at(4).get<std::string>()) / 24 / 60 / 60);
    setBudgetPerPeriod(json_proposal_info.at(5).get<std::string>());
    setInitialPeriod(std::stoul(json_proposal_info.at(6).get<std::string>()));
    setContestPeriod(std::stoul(json_proposal_info.at(7).get<std::string>()));

    // Set "data" part
    setNumPeriodsLeft(std::stoul(json_proposal_data.at(0).get<std::string>()));
    setNextPaymentDate(std::stoul(json_proposal_data.at(1).get<std::string>()));
    setStatus(static_cast<Proposal::Status>(std::stoul(json_proposal_data.at(2).get<std::string>())));
    setInitialVotingEndDate(std::stoul(json_proposal_data.at(3).get<std::string>()));
    setInitialContestEndDate(std::stoul(json_proposal_data.at(4).get<std::string>()));
}

String Proposal::getStatusStr(Proposal::Status status) {
  switch (status) {
    case Proposal::Status::Uninitialized: return "Uninitialized";
    case Proposal::Status::Started: return "Voting";
    case Proposal::Status::Accepted: return "Accepted";
    case Proposal::Status::Contested: return "Contested";
    case Proposal::Status::Rejected: return "Rejected";
    case Proposal::Status::Completed: return "Completed";
    case Proposal::Status::PrepayingGas: return "Prepaying Gas";
    default: return "Unknown";
  }
}

static float getPercentage(const String& dividend, const String& divisor) {
  BigInteger reminder;
  BigInteger dividendInt;
  dividendInt.parseString(dividend, 10);
  BigInteger divisorInt;
  divisorInt.parseString(divisor, 10);

  dividendInt.divideBy(divisorInt, reminder);
  return dividendInt.toInteger() / 100.f;
}

float Proposal::getSpentPrecent() const {
  return getPercentage(m_amountSpent, m_budgetPerPeriod);
}

float Proposal::getBounusPrecent() const {
  return getPercentage(m_targetBonus, m_budgetPerPeriod);
}

bool Proposal::isRewardClaimable() const noexcept {
  // TODO(Kirill) need to think which statuses are eligible for claiming (perhaps, Accepted, Completed as well)
  // Temporarily enable claiming reward button at "Started" state as well. That's because proposal state is updated
  // in smart contract only before each vote. So the proposal can have 100% approval rate but still remain in
  // "Started" state until some action that calls updateProposalState() smart-contract function is performed.
  const bool isClaimingActive = getStatus() == Proposal::Status::Accepted
                                || getStatus() == Proposal::Status::Started;
  return isClaimingActive;
}

void Proposal::setSlots(const Array<uint64>& slots, NotificationType notify) {
  m_slots = slots;

  if (notify != NotificationType::dontSendNotification)
    notifyChanged();
}

void Proposal::setInitialVotingEndDate(uint64 dateUnix) {
  m_initialVotingEndDate = Time(dateUnix * 1000);
}

void Proposal::setInitialContestEndDate(uint64 dateUnix) {
  m_initialContestEndDate = Time(dateUnix * 1000);
}

void Proposal::setNextPaymentDate(uint64 dateUnix) {
  m_nextPaymentDate = Time(dateUnix * 1000);
}

bool Proposal::hasActiveStatus() const noexcept {
  return getStatus() == Proposal::Status::Started
         || getStatus() == Proposal::Status::Accepted
         || getStatus() == Proposal::Status::Contested;
}

bool Proposal::hasInactiveStatus() const noexcept {
  return !hasActiveStatus();
}
