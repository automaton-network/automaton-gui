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
  : m_id (0)
  , m_amountSpent (0)
  , m_budget (0)
  , m_numPeriods (0)
  , m_targetBonus (0)
  , m_approvalRating (0)
  , m_lengthDays (0)
  , m_timeLeft (0)
  , m_status (Proposal::Status::Uninitialized)
{
}

Proposal::Proposal (uint32_t id, const String& jsonString)
  : m_id (0)
  , m_amountSpent (0)
  , m_budget (0)
  , m_numPeriods (0)
  , m_targetBonus (0)
  , m_approvalRating (0)
  , m_lengthDays (0)
  , m_timeLeft (0)
  , m_status (Proposal::Status::Uninitialized)
{
    setId (id);

    json json_proposal = json::parse (jsonString.toStdString());
    //TODO: add json error handling

    setCreator (json_proposal.at (0).get<std::string>());
    setTitle (json_proposal.at (1).get<std::string>());
    setDocumentLink (json_proposal.at (2).get<std::string>());
    setDocumentHash (json_proposal.at (3).get<std::string>());
    setLengthDays (std::stoul (json_proposal.at (4).get<std::string>()));
    setNumPeriods (std::stoul (json_proposal.at (5).get<std::string>()));
    setBudget (std::stoul (json_proposal.at (6).get<std::string>()));
    //TODO
    //setNextPaymentDate (json_proposal.at (7.get<uint64_t>()));

    setStatus (static_cast<Proposal::Status> (std::stoul (json_proposal.at(8).get<std::string>())));
    //TODO
    //setInitialEndDate (json_proposal.at (9).get<uint64_t>());
    //setContestEndDate (json_proposal.at (10).get<uint64_t>());
}

String Proposal::getStatusStr (Proposal::Status status)
{
  switch (status)
  {
    case Proposal::Status::Uninitialized: return "Uninitialized";
    case Proposal::Status::Started: return "Voting";
    case Proposal::Status::Accepted: return "Accepted";
    case Proposal::Status::Contested: return "Contested";
    case Proposal::Status::Rejected: return "Rejected";
    case Proposal::Status::Completed: return "Completed";
    default: return "Unknown";
  }
}
