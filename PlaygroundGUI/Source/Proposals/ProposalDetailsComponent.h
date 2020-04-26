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
class ProposalDetailsComponent : public Component, public Button::Listener {
 public:
  ProposalDetailsComponent(Account::Ptr accountData);
  ~ProposalDetailsComponent();

  void setProposal(Proposal::Ptr proposal);

  void paint(Graphics&) override;
  void resized() override;
  void buttonClicked(Button* button) override;

 private:
  Proposal::Ptr m_proposal;
  Label m_title;
  Label m_reward;
  Label m_status;
  std::unique_ptr<HyperlinkButton> m_linkToDocument;
  std::unique_ptr<TextButton> m_backBtn;

  Account::Ptr m_accountData;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProposalDetailsComponent)
};
