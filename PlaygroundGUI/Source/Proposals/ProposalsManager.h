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

#include "ProposalsModel.h"

class ProposalsManager : public DeletedAtShutdown {
 public:
  std::shared_ptr<ProposalsModel> getModel() const { return m_model; }

  ProposalsManager();
  ~ProposalsManager();

  bool fetchProposals();
  bool addProposal(const Proposal& proposal, bool sendNotification = true);
  bool createProposal(Proposal::Ptr proposal, const std::string& contributor);
  bool payForGas(Proposal::Ptr proposal, uint64 slotsToPay);
  bool castVote(Proposal::Ptr proposal, uint64 choice);

  void notifyProposalsUpdated();

  JUCE_DECLARE_SINGLETON(ProposalsManager, true)


 private:
  std::shared_ptr<ProposalsModel> m_model;
};
