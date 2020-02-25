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

#include "ProposalsPage.h"
#include "ProposalsManager.h"


class ProposalsUIModel : public TableListBoxModel
{
public:
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

  ProposalsUIModel()
  {
  }

  void setModel (std::shared_ptr<AbstractListModel<Proposal::Ptr>> model)
  {
    m_model = model;
  }

  int getNumRows() override
  {
    return m_model != nullptr ? m_model->size() : 0;
  }

  void paintCell (Graphics& g,
                  int rowNumber,
                  int columnId,
                  int width,
                  int height,
                  bool rowIsSelected) override
  {
    auto item = m_model->getAt (rowNumber);
    g.setColour (Colours::white);

    auto getPercentStr = [] (float percent) -> String { return String (percent * 100) + "%"; };

    switch (columnId)
    {
      case ID:
        {
          g.drawText (String (item->getId()), 0, 0, width, height, Justification::centred);
          break;
        }
      case CreatorAndTitle:
        {
          g.drawText (String (item->getTitle()), 0, height / 2, width, height / 2, Justification::centredLeft);
          g.setFont (g.getCurrentFont().boldened());
          g.drawText (String (item->getCreator()), 0, 0, width, height / 2, Justification::centredLeft);
          break;
        }
      case ApprovalRating:
        {
          if (item->getApprovalRating() < 0)
            g.setColour (Colours::red);
          else
            g.setColour (Colours::green);

          g.drawText (String (item->getApprovalRating()) + "%", 0, 0, width, height, Justification::centred);
          break;
        }
      case Status:
        {
          g.drawText (Proposal::getStatusStr (item->getStatus()), 0, 0, width, height, Justification::centred);
          break;
        }
      case Spent:
        {
          const auto percent = item->getBudget() ? item->getAmountSpent() / item->getBudget() : 0.f;
          const String text = String (item->getAmountSpent()) + " (" + getPercentStr(percent) + ")";
          g.drawText (text, 0, 0, width, height, Justification::centred);
          break;
        }
      case Budget:
        {
          g.drawText (String (item->getBudget()), 0, 0, width, height, Justification::centred);
          break;
        }
      case Periods:
        {
          g.drawText (String (item->getNumPeriods()), 0, 0, width, height, Justification::centred);
          break;
        }
      case Length:
        {
          const auto lengthDays = item->getLengthDays();
          // If length is zero - the proposal has no time limit
          if (lengthDays)
          {
            const String unit (lengthDays == 1 ? "day" : "days");
            g.drawText (String (lengthDays) + " " + unit, 0, 0, width, height, Justification::centred);
          }
          break;
        }
      case Bonus:
        {
          const auto percent = item->getBudget() ? item->getTargetBonus() / item->getBudget() : 0.f;
          const String text = String (item->getTargetBonus()) + " (" + getPercentStr(percent) + ")";
          g.drawText (text, 0, 0, width, height, Justification::centred);
          break;
        }
      case TimeLeft:
        {
          const String unit (item->getLengthDays() == 1 ? "day" : "days");
          g.drawText (String (item->getTimeLeftDays()) + " " + unit, 0, 0, width, height, Justification::centred);
          break;
        }
      default:
      break;
    }
  }

  void paintRowBackground (Graphics& g,
                           int rowNumber,
                           int width,
                           int height,
                           bool rowIsSelected) override
  {
    auto colour = LookAndFeel::getDefaultLookAndFeel().findColour (TableListBox::backgroundColourId);
    g.setColour (rowIsSelected ? colour.darker (0.3f) : colour);
    g.fillRect (0, 0, width, height);
  }

private:
  std::shared_ptr<AbstractListModel<Proposal::Ptr>> m_model;
};

//==============================================================================
ProposalsPage::ProposalsPage()
{
  m_proposalsListBox = std::make_unique<TableListBox>();
  m_proposalsListBox->setRowHeight (50);
  m_proposalsListBox->setRowSelectedOnMouseDown (true);
  auto& tableHeader = m_proposalsListBox->getHeader();
  tableHeader.setStretchToFitActive (true);
  tableHeader.addColumn (translate ("ID"), ProposalsUIModel::ID, 30, 30, 50);
  tableHeader.addColumn (translate ("Creator/Title"), ProposalsUIModel::CreatorAndTitle, 200);
  tableHeader.addColumn (translate ("Approval Rating"), ProposalsUIModel::ApprovalRating, 125);
  tableHeader.addColumn (translate ("Status"), ProposalsUIModel::Status, 75);
  tableHeader.addColumn (translate ("Spent"), ProposalsUIModel::Spent, 75);
  tableHeader.addColumn (translate ("Budget"), ProposalsUIModel::Budget, 75);
  tableHeader.addColumn (translate ("Periods"), ProposalsUIModel::Periods, 75);
  tableHeader.addColumn (translate ("Length"), ProposalsUIModel::Length, 75);
  tableHeader.addColumn (translate ("Target Bonus"), ProposalsUIModel::Bonus, 75);
  tableHeader.addColumn (translate ("Time Left"), ProposalsUIModel::TimeLeft, 75);

  m_createProposalBtn = std::make_unique<TextButton> (translate ("Create Proposal"));
  m_createProposalBtn->addListener (this);
  m_payForGasBtn = std::make_unique<TextButton> (translate ("Pay for Gas"));
  m_FilterBtn = std::make_unique<TextButton> (translate ("Filter..."));
  m_abandonProposalBtn = std::make_unique<TextButton> (translate ("Abandon Proposal"));

  m_filterByStatusComboBox = std::make_unique<ComboBox>();
  m_filterByStatusComboBox->addItem (translate ("All"), 1);
  m_filterByStatusComboBox->addItem (translate ("Paying Gas"), 2);
  m_filterByStatusComboBox->addItem (translate ("Approved"), 3);
  m_filterByStatusComboBox->addItem (translate ("Accepted"), 4);
  m_filterByStatusComboBox->addItem (translate ("Rejected"), 5);
  m_filterByStatusComboBox->addItem (translate ("Contested"), 6);
  m_filterByStatusComboBox->addItem (translate ("Completed"), 7);

  m_proxyModel = std::make_shared<ProposalsProxyModel>();
  m_proxyModel->addListener (this);

  m_proposalsUIModel = std::make_unique<ProposalsUIModel>();
  m_proposalsUIModel->setModel (m_proxyModel);
  m_proposalsListBox->setModel (m_proposalsUIModel.get());

  // Enable only when any proposal is selected
  m_payForGasBtn->setEnabled (false);
  m_abandonProposalBtn->setEnabled (false);

  m_createProposalView = std::make_unique<CreateProposalComponent>();
  m_createProposalView->addListener (this);
  addChildComponent (m_createProposalView.get());

  addAndMakeVisible (m_createProposalBtn.get());
  addAndMakeVisible (m_payForGasBtn.get());
  addAndMakeVisible (m_FilterBtn.get());
  addAndMakeVisible (m_abandonProposalBtn.get());
  addAndMakeVisible (m_filterByStatusComboBox.get());
  addAndMakeVisible (m_proposalsListBox.get());
}

ProposalsPage::~ProposalsPage()
{
}

void ProposalsPage::paint (Graphics&)
{
}

void ProposalsPage::resized()
{
  auto bounds = getLocalBounds().reduced (20);
  m_createProposalView->setBounds(bounds);
  m_proposalsListBox->setBounds (bounds.removeFromTop (500));
  m_proposalsListBox->getHeader().resizeAllColumnsToFit (bounds.getWidth());
  bounds.removeFromTop (20);

  const int buttonsWidth = 150;
  const int buttonsHeight = 50;
  const int buttonsSpacing = 10;
  auto buttonsArea = bounds.removeFromTop (buttonsHeight);
  m_createProposalBtn->setBounds (buttonsArea.removeFromLeft (buttonsWidth));
  buttonsArea.removeFromLeft (buttonsSpacing);
  m_payForGasBtn->setBounds (buttonsArea.removeFromLeft (buttonsWidth));
  buttonsArea.removeFromLeft (buttonsSpacing);
  m_abandonProposalBtn->setBounds (buttonsArea.removeFromLeft (buttonsWidth));
  buttonsArea.removeFromLeft (buttonsSpacing);
  m_filterByStatusComboBox->setBounds (buttonsArea.removeFromLeft (buttonsWidth));
  buttonsArea.removeFromLeft (buttonsSpacing);
  m_FilterBtn->setBounds (buttonsArea.removeFromLeft (buttonsWidth));
}

void ProposalsPage::setModel (std::shared_ptr<ProposalsModel> model)
{
  m_proxyModel->setModel (model);
}

void ProposalsPage::modelChanged (AbstractListModelBase*)
{
  m_proposalsListBox->updateContent();
}

void ProposalsPage::createProposalViewActionHappened (CreateProposalComponent* componentInWhichActionHappened, CreateProposalComponent::Action action)
{
  if (action == CreateProposalComponent::Action::ProposalCreated)
  {
    //TODO
    componentInWhichActionHappened->setVisible (false);
    ProposalsManager::getInstance()->addProposal (componentInWhichActionHappened->getProposal(), "a6C8015476f6F4c646C95488c5fc7f5174A4E0ef");
  }
  else if (action == CreateProposalComponent::Action::Cancelled)
  {
    componentInWhichActionHappened->setVisible (false);
  }
}

void ProposalsPage::buttonClicked (Button* buttonThatWasClicked)
{
  if (buttonThatWasClicked == m_createProposalBtn.get())
  {
    m_createProposalView->setVisible (true);
    m_createProposalView->setAlwaysOnTop (true);
  }
}
