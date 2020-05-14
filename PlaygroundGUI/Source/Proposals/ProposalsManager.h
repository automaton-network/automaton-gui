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

#include <Utils/OwnedTasks.h>
#include "../Login/Account.h"
#include "ProposalsModel.h"

class ProposalsManager : public OwnedTasks {
 public:
  std::shared_ptr<ProposalsModel> getModel() const { return m_model; }

  ProposalsManager(Account::Ptr accountData);
  ~ProposalsManager();

  bool fetchProposals();
  bool fetchProposalVotes(Proposal::Ptr proposal);
  bool updateProposal(Proposal::Ptr proposal);
  bool createProposal(Proposal::Ptr proposal, const String& contributor);
  bool payForGas(Proposal::Ptr proposal, uint64 slotsToPay);
  bool castVote(Proposal::Ptr proposal, uint64 choice);
  bool claimReward(Proposal::Ptr proposal, const String& rewardAmount);

  std::string getEthAddress() const noexcept { return m_accountData->getAddress(); }
  std::string getEthAddressAlias() const noexcept { return m_accountData->getAlias(); }

 private:
  std::shared_ptr<ProposalsModel> m_model;

  Account::Ptr m_accountData;
  std::shared_ptr<AutomatonContractData> m_contractData;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProposalsManager)
};
