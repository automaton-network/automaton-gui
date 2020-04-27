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

#include <JuceHeader.h>
#include <memory>
#include <string>


class Proposal {
 public:
  class Listener {
   public:
    virtual ~Listener() = default;
    virtual void proposalChanged() = 0;
  };

  enum class Status {
    Uninitialized = 0  // there is no proposal connected to this ballot
    // ACTIVE statuses
    , Started    // first period of voting until initialEndDate
    , Accepted   // enough "yes" votes during the initial voting period
    , Contested  // too many "no" votes after the first approval ot the proposal
    // INACTIVE statuses
    , Rejected   // not enough "yes" votes during the initial period or during a contest period
    , Completed  // the proposal was successfully implemented
    , PrepayingGas  // the proposal is inactive until all slots are paid
  };

  using Ptr = std::shared_ptr<Proposal>;

  Proposal();
  Proposal(uint32_t id, const String& jsonString);

  static String getStatusStr(Proposal::Status status);

  void setId(uint64 id)                             { m_id = id; }
  void setAmountSpent(const String& amountSpent)    { m_amountSpent = amountSpent; }
  void setBudget(const String& budget)              { m_budget = budget; }
  void setNumPeriods(uint64 numPeriods)             { m_numPeriods = numPeriods; }
  void setTargetBonus(const String& targetBonus)    { m_targetBonus = targetBonus; }
  void setLengthDays(uint64 lengthDays)             { m_lengthDays = lengthDays; }
  void setApprovalRating(int approvalRating)        { m_approvalRating = approvalRating; }
  void setTimeLeftDays(int timeLeftDays)            { m_timeLeft = timeLeftDays; }
  void setNumSlotsPaid(uint64 numSlotsPaid)         { m_numSlotsPaid = numSlotsPaid; }
  void setAllSlotsPaid(bool allSlotsPaid)           { m_areAllSlotsPaid = allSlotsPaid; }
  void setTitle(const String& title)                { m_title = title; }
  void setCreator(const String& creator)            { m_creator = creator; }
  void setCreatorAlias(const String& creatorAlias)  { m_creatorAlias = creatorAlias; }
  void setDocumentLink(const String& documentLink)  { m_documentLink = documentLink; }
  void setDocumentHash(const String& documentHash)  { m_documentHash = documentHash; }
  void setStatus(Proposal::Status status)           { m_status = status; }

  void setSlots(const Array<uint64>& slots, NotificationType notify);
  void setInitialVotingEndDate(uint64 dateUnix);
  void setInitialContestEndDate(uint64 dateUnix);
  void setNextPaymentDate(uint64 dateUnix);

  bool isRewardClaimable() const noexcept;

  uint64 getId() const noexcept               { return m_id; }
  String getAmountSpent() const noexcept      { return m_amountSpent; }
  String getBudget() const noexcept           { return m_budget; }
  uint64 getNumPeriods() const  noexcept      { return m_numPeriods; }
  String getTargetBonus() const noexcept      { return m_targetBonus; }
  uint64 getLengthDays() const noexcept       { return m_lengthDays; }
  int getApprovalRating() const noexcept      { return m_approvalRating; }
  int getTimeLeftDays() const noexcept        { return m_timeLeft; }
  uint64 getNumSlotsPaid() const noexcept     { return m_numSlotsPaid; }
  bool areAllSlotsPaid() const noexcept       { return m_areAllSlotsPaid; }
  String getTitle() const noexcept            { return m_title; }
  String getCreator() const noexcept          { return m_creator; }
  String getCreatorAlias() const noexcept     { return m_creatorAlias; }
  String getDocumentLink() const noexcept     { return m_documentLink; }
  String getDocumentHash() const noexcept     { return m_documentHash; }
  Proposal::Status getStatus() const noexcept { return m_status; }

  Array<uint64> getSlots() const              { return m_slots; }
  Time getInitialVotingEndDate() const noexcept     { return m_initialVotingEndDate; }
  Time getInitialContestEndDate() const noexcept    { return m_initialContestEndDate; }
  Time getNextPaymentDate() const noexcept          { return m_nextPaymentDate; }

  float getSpentPrecent() const;
  float getBounusPrecent() const;

  void addListener(Listener* listener) {
    m_listeners.add(listener);
  }

  void removeListener(Listener* listener) {
    m_listeners.remove(listener);
  }

  void notifyChanged() {
    m_listeners.call(&Listener::proposalChanged);
  }

 private:
  uint64 m_id;
  String m_amountSpent;
  String m_budget;
  uint64 m_numPeriods;
  String m_targetBonus;
  uint32 m_lengthDays;

  int32 m_approvalRating;
  int32 m_timeLeft;

  uint32 m_numSlotsPaid;
  bool m_areAllSlotsPaid;

  String m_title = "Test title";
  String m_creator = "Test creator";
  String m_creatorAlias;
  String m_documentLink;
  String m_documentHash;

  Time m_initialVotingEndDate;
  Time m_initialContestEndDate;
  Time m_nextPaymentDate;

  Proposal::Status m_status;
  Array<uint64> m_slots;

  ListenerList<Listener> m_listeners;
};
