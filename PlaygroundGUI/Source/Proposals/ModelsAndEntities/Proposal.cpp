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
