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

#include "../../JuceLibraryCode/JuceHeader.h"
#include "Proposal.h"


class CreateProposalComponent : public Component
                              , private TextEditor::Listener
                              , private Button::Listener
                              , private Proposal::Listener {
 public:
  enum class Action {
    ProposalCreated = 1
    , Cancelled
    , Unknown
  };

  class Listener {
   public:
    virtual ~Listener() {}
    virtual void createProposalViewActionHappened(CreateProposalComponent* componentInWhichActionHappened,
                                                  CreateProposalComponent::Action action) = 0;
  };

  CreateProposalComponent();
  ~CreateProposalComponent();

  Proposal::Ptr getProposal() const { return m_proposal; }

  void addListener(CreateProposalComponent::Listener* listener) { m_listeners.add(listener); }
  void removeListener(CreateProposalComponent::Listener* listener) { m_listeners.remove(listener); }

  void paint(Graphics& g) override;
  void resized() override;


 private:
  void buttonClicked(Button* buttonThatWasClicked) override;
  void textEditorTextChanged(TextEditor& editor) override;
  void proposalChanged() override;

  void clearFields();

  std::unique_ptr<TextEditor> m_titleEditor;
  std::unique_ptr<TextEditor> m_budgetEditor;
  std::unique_ptr<TextEditor> m_totalBudgetEditor;
  std::unique_ptr<TextEditor> m_numPeriodsEditor;
  std::unique_ptr<TextEditor> m_targetBonusEditor;
  std::unique_ptr<TextEditor> m_lengthDaysEditor;

  std::unique_ptr<TextButton> m_createProposalBtn;
  std::unique_ptr<TextButton> m_cancelBtn;

  Proposal::Ptr m_proposal;

  ListenerList<CreateProposalComponent::Listener> m_listeners;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CreateProposalComponent);
};
