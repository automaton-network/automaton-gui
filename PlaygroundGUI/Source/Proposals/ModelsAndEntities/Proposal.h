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

#pragma once
#include "../../../JuceLibraryCode/JuceHeader.h"

class Proposal
{
public:
  enum class Status
  {
    Uninitialized = 0   // there is no proposal connected to this ballot
    // ACTIVE statuses
    , Started   // first period of voting until initialEndDate
    , Accepted  // enough "yes" votes during the initial voting period
    , Contested // too many "no" votes after the first approval ot the proposal
    // INACTIVE statuses
    , Rejected  // not enough "yes" votes during the initial period or during a contest period
    , Completed // the proposal was successfully implemented
  };

  Proposal();
  using Ptr = std::shared_ptr<Proposal>;

  static String getStatusStr(Status status);

  void setId          (uint64 id)             { m_id = id; }
  void setAmountSpent (uint64 amountSpent)    { m_amountSpent = amountSpent; }
  void setBudget      (uint64 budget)         { m_budget = budget; }
  void setNumPeriods  (uint64 numPeriods)     { m_numPeriods = numPeriods; }
  void setTargetBonus (uint64 targetBonus)    { m_targetBonus = targetBonus; }
  void setLengthDays  (uint64 lengthDays)     { m_lengthDays = lengthDays; }
  void setApprovalRating  (int approvalRating)    { m_approvalRating = approvalRating; }
  void setTimeLeftDays    (int timeLeftDays)      { m_timeLeft = timeLeftDays; }
  void setTitle   (const String& title)   { m_title = title; }
  void setCreator (const String& creator) { m_creator = creator; }
  void setStatus (Status status) { m_status = status; }

  uint64 getId() const            { return m_id; }
  uint64 getAmountSpent() const   { return m_amountSpent; }
  uint64 getBudget() const        { return m_budget; }
  uint64 getNumPeriods() const    { return m_numPeriods; }
  uint64 getTargetBonus() const   { return m_targetBonus; }
  uint64 getLengthDays() const    { return m_lengthDays; }
  int getApprovalRating() const   { return m_approvalRating; }
  int getTimeLeftDays() const     { return m_timeLeft; }
  String getTitle() const         { return m_title; }
  String getCreator() const       { return m_creator; }
  Status getStatus() const        { return m_status; }


private:
  uint64 m_id;
  uint64 m_amountSpent;
  uint64 m_budget;
  uint64 m_numPeriods;
  uint64 m_targetBonus;
  uint32 m_lengthDays;
  int m_approvalRating;
  int m_timeLeft;
  String m_title = "Test title";
  String m_creator = "Test creator";
  Status m_status;
};
