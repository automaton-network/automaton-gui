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
#include "ModelsAndEntities/ProposalsModel.h"
#include "CreateProposalComponent.h"

class ProposalsUIModel;

class ProposalsPage : public Component
                    , public AbstractListModelBase::Listener
                    , private CreateProposalComponent::Listener
                    , private Button::Listener
                    , private ComboBox::Listener
                    , private TableListBoxModel
{
public:
  ProposalsPage();
  ~ProposalsPage();

  void setModel (std::shared_ptr<ProposalsModel> model);

  void paint (Graphics&) override;
  void resized() override;

  // AbstractListModelBase::Listener
  void modelChanged (AbstractListModelBase*);

private:
  void createProposalViewActionHappened (CreateProposalComponent* componentInWhichActionHappened, CreateProposalComponent::Action action) override;

  void buttonClicked (Button* buttonThatWasClicked) override;
  void comboBoxChanged (ComboBox* comboBoxThatHasChanged) override;
  void updateButtons();

  // TableListBoxModel
  //==============================================================================
  enum Columns
  {
    ID = 1
    , CreatorAndTitle
    , ApprovalRating
    , Status
    , Spent
    , Budget
    , Periods
    , Length
    , Bonus
    , TimeLeft
  };

  int getNumRows() override;
  void sortOrderChanged (int columnId, bool isForwards) override;
  void paintCell (Graphics& g,
          int rowNumber,
          int columnId,
          int width,
          int height,
          bool rowIsSelected) override;
  void paintRowBackground (Graphics& g,
                           int rowNumber,
                           int width,
                           int height,
                           bool rowIsSelected) override;
  void selectedRowsChanged (int lastRowSelected) override;
  //==============================================================================

  std::unique_ptr<TableListBox> m_proposalsListBox;
  std::unique_ptr<TextButton> m_createProposalBtn;
  std::unique_ptr<TextButton> m_payForGasBtn;
  std::unique_ptr<TextButton> m_abandonProposalBtn;
  std::unique_ptr<TextButton> m_FilterBtn;
  std::unique_ptr<ComboBox> m_filterByStatusComboBox;

  std::unique_ptr<CreateProposalComponent> m_createProposalView;
  std::shared_ptr<ProposalsProxyModel> m_proxyModel;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProposalsPage);
};
