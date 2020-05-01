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
#include <Proposals/Proposal.h>
#include <Login/Account.h>
#include "../Components/ValidatorGrid.h"

//==============================================================================
/*
*/

class VoteSlotsGrid;

class ProposalDetailsComponent : public Component
  , public Button::Listener
  , public Proposal::Listener
  , public AsyncUpdater {
 public:
  ProposalDetailsComponent(Account::Ptr accountData);
  ~ProposalDetailsComponent();

  void setProposal(Proposal::Ptr proposal);
  void updateButtonsForProposal();
  void updateVotesView();

  void paint(Graphics&) override;
  void resized() override;
  void buttonClicked(Button* button) override;
  void proposalChanged() override;
  void handleAsyncUpdate() override;

 private:
  Proposal::Ptr m_proposal;
  Label m_title;
  Label m_proposalDetailsLabel;
  std::unique_ptr<HyperlinkButton> m_linkToDocument;
  std::unique_ptr<TextButton> m_backBtn;
  std::unique_ptr<VoteSlotsGrid> m_slotsGrid;

  std::unique_ptr<TextButton> m_payForGasBtn;
  std::unique_ptr<TextButton> m_voteYesBtn;
  std::unique_ptr<TextButton> m_voteNoBtn;
  std::unique_ptr<TextButton> m_unspecifiedBtn;
  std::unique_ptr<TextButton> m_claimRewardBtn;

  Account::Ptr m_accountData;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProposalDetailsComponent)
};
