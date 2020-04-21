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
#include "../Utils/Utils.h"
#include "../Data/AutomatonContractData.h"

//==============================================================================
ProposalsPage::ProposalsPage(Account::Ptr accountData)
    : m_tooltipWindow(this, 500)
    , m_accountData(accountData) {
  m_proposalsManager = m_accountData->getProposalsManager();

  m_proposalsListBox = std::make_unique<TableListBox>();
  m_proposalsListBox->setRowHeight(50);
  m_proposalsListBox->setRowSelectedOnMouseDown(true);
  m_proposalsListBox->setClickingTogglesRowSelection(true);

  auto& tableHeader = m_proposalsListBox->getHeader();
  tableHeader.setStretchToFitActive(true);
  tableHeader.addColumn(translate("ID"), Columns::ID, 30, 30, 50);
  tableHeader.addColumn(translate("Creator/Title"), Columns::CreatorAndTitle, 200);
  tableHeader.addColumn(translate("Approval Rating"), Columns::ApprovalRating, 125);
  tableHeader.addColumn(translate("Status"), Columns::Status, 75);
  tableHeader.addColumn(translate("Spent"), Columns::Spent, 75);
  tableHeader.addColumn(translate("Reward p/period"), Columns::Budget, 75);
  tableHeader.addColumn(translate("Periods left"), Columns::Periods, 75);
  tableHeader.addColumn(translate("Length"), Columns::Length, 75);
  tableHeader.addColumn(translate("Target Bonus"), Columns::Bonus, 75);
  tableHeader.addColumn(translate("Time Left"), Columns::TimeLeft, 75);

  // Temporarily disable unused columns (we can't receive a correct data from the contract at the moment)
  tableHeader.setColumnVisible(Columns::Spent, false);
  tableHeader.setColumnVisible(Columns::Bonus, false);
  tableHeader.setColumnVisible(Columns::TimeLeft, false);

  m_fetchProposalsBtn = std::make_unique<TextButton>(translate("Fetch Proposals"));
  m_fetchProposalsBtn->addListener(this);
  m_createProposalBtn = std::make_unique<TextButton>(translate("Create Proposal"));
  m_createProposalBtn->addListener(this);
  m_payForGasBtn = std::make_unique<TextButton>(translate("Pay for Gas"));
  m_payForGasBtn->addListener(this);
  m_FilterBtn = std::make_unique<TextButton>(translate("Filter..."));
  m_voteYesBtn = std::make_unique<TextButton>(translate("Vote YES"));
  m_voteYesBtn->addListener(this);
  m_voteNoBtn = std::make_unique<TextButton>(translate("Vote NO"));
  m_voteNoBtn->addListener(this);
  m_claimRewardBtn = std::make_unique<TextButton>(translate("Claim reward"));
  m_claimRewardBtn->addListener(this);
  m_abandonProposalBtn = std::make_unique<TextButton>(translate("Abandon Proposal"));

  m_filterByStatusComboBox = std::make_unique<ComboBox>();
  m_filterByStatusComboBox->addItem(translate("All"), static_cast<int>(ProposalFilter::All));
  m_filterByStatusComboBox->addItem(translate("Paying Gas"), static_cast<int>(ProposalFilter::PayingGas));
  m_filterByStatusComboBox->addItem(translate("Approved"), static_cast<int>(ProposalFilter::Approved));
  m_filterByStatusComboBox->addItem(translate("Accepted"), static_cast<int>(ProposalFilter::Accepted));
  m_filterByStatusComboBox->addItem(translate("Rejected"), static_cast<int>(ProposalFilter::Rejected));
  m_filterByStatusComboBox->addItem(translate("Contested"), static_cast<int>(ProposalFilter::Contested));
  m_filterByStatusComboBox->addItem(translate("Completed"), static_cast<int>(ProposalFilter::Completed));
  m_filterByStatusComboBox->addListener(this);

  m_proxyModel = std::make_shared<ProposalsProxyModel>();
  m_proxyModel->addListener(this);
  m_proposalsListBox->setModel(this);

  // Enable only when any proposal is selected
  m_payForGasBtn->setEnabled(false);
  m_abandonProposalBtn->setEnabled(false);
  m_voteYesBtn->setEnabled(false);
  m_voteNoBtn->setEnabled(false);
  m_claimRewardBtn->setEnabled(false);

  m_createProposalView = std::make_unique<CreateProposalComponent>();
  m_createProposalView->addListener(this);
  addChildComponent(m_createProposalView.get());

  addAndMakeVisible(m_fetchProposalsBtn.get());
  addAndMakeVisible(m_createProposalBtn.get());
  addAndMakeVisible(m_payForGasBtn.get());
  addAndMakeVisible(m_FilterBtn.get());
  addAndMakeVisible(m_voteYesBtn.get());
  addAndMakeVisible(m_voteNoBtn.get());
  addAndMakeVisible(m_claimRewardBtn.get());
  addAndMakeVisible(m_filterByStatusComboBox.get());
  addAndMakeVisible(m_proposalsListBox.get());

  setModel(m_proposalsManager->getModel());
}

ProposalsPage::~ProposalsPage() {
}

void ProposalsPage::paint(Graphics&) {
}

void ProposalsPage::resized() {
  auto bounds = getLocalBounds().reduced(20);
  m_createProposalView->setBounds(bounds);

  const int buttonsHeight = 50;
  const int buttonsSpacing = 10;
  const int buttonsWidth = 120;
  auto buttonsArea = bounds.removeFromBottom(buttonsHeight);
  m_fetchProposalsBtn->setBounds(buttonsArea.removeFromLeft(buttonsWidth));
  buttonsArea.removeFromLeft(buttonsSpacing);
  m_createProposalBtn->setBounds(buttonsArea.removeFromLeft(buttonsWidth));
  buttonsArea.removeFromLeft(buttonsSpacing);
  m_payForGasBtn->setBounds(buttonsArea.removeFromLeft(buttonsWidth));
  buttonsArea.removeFromLeft(buttonsSpacing);
  m_voteYesBtn->setBounds(buttonsArea.removeFromLeft(buttonsWidth));
  buttonsArea.removeFromLeft(buttonsSpacing);
  m_voteNoBtn->setBounds(buttonsArea.removeFromLeft(buttonsWidth));
  buttonsArea.removeFromLeft(buttonsSpacing);
  m_claimRewardBtn->setBounds(buttonsArea.removeFromLeft(buttonsWidth));
  buttonsArea.removeFromLeft(buttonsSpacing);
  m_filterByStatusComboBox->setBounds(buttonsArea.removeFromLeft(buttonsWidth));
  buttonsArea.removeFromLeft(buttonsSpacing);
  m_FilterBtn->setBounds(buttonsArea.removeFromLeft(buttonsWidth));

  bounds.removeFromBottom(buttonsSpacing * 2);
  m_proposalsListBox->setBounds(bounds);
  m_proposalsListBox->getHeader().resizeAllColumnsToFit(bounds.getWidth());
}

void ProposalsPage::setModel(std::shared_ptr<ProposalsModel> model) {
  m_proxyModel->setModel(model);
}

void ProposalsPage::modelChanged(AbstractListModelBase*) {
  m_proposalsListBox->updateContent();
  m_proposalsListBox->repaint();
}

void ProposalsPage::createProposalViewActionHappened(CreateProposalComponent* componentInWhichActionHappened,
                                                     CreateProposalComponent::Action action) {
  if (action == CreateProposalComponent::Action::ProposalCreated) {
    componentInWhichActionHappened->setVisible(false);
    auto proposal = componentInWhichActionHappened->getProposal();
    proposal->setCreatorAlias(m_proposalsManager->getEthAddressAlias());
    proposal->setCreator(m_proposalsManager->getEthAddress());
    m_proposalsManager->createProposal(proposal,
                                       proposal->getCreator());
  } else if (action == CreateProposalComponent::Action::Cancelled) {
    componentInWhichActionHappened->setVisible(false);
  }
}

void ProposalsPage::buttonClicked(Button* buttonThatWasClicked) {
  if (buttonThatWasClicked == m_createProposalBtn.get()) {
    m_createProposalView->setVisible(true);
    m_createProposalView->setAlwaysOnTop(true);
  } else if (buttonThatWasClicked == m_voteYesBtn.get()) {
    auto proposal = m_proxyModel->getAt(m_proposalsListBox->getSelectedRow());
    m_proposalsManager->castVote(proposal, 1);
  } else if (buttonThatWasClicked == m_voteNoBtn.get()) {
    auto proposal = m_proxyModel->getAt(m_proposalsListBox->getSelectedRow());
    m_proposalsManager->castVote(proposal, 2);
  } else if (buttonThatWasClicked == m_payForGasBtn.get()) {
    auto proposal = m_proxyModel->getAt(m_proposalsListBox->getSelectedRow());
    const auto numSlots = m_accountData->getContractData()->getSlotsNumber();
    const auto numSlotsPaid = proposal->getNumSlotsPaid();
    const String slotsMsg = String(numSlotsPaid) + String("/") + String(numSlots) + String(" are paid. ")
                              + String("Enter num of slots to pay");
    AlertWindow w("Pay for gas for " + proposal->getTitle() + " proposal",
                  slotsMsg,
                  AlertWindow::QuestionIcon);

    w.addTextEditor("slotsToPay", "", "Slots to pay:", false);
    w.addButton("OK", 1, KeyPress(KeyPress::returnKey, 0, 0));
    w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

    if (w.runModalLoop() == 1) {
      auto slotsToPay = w.getTextEditorContents("slotsToPay").getLargeIntValue();
      if (slotsToPay > 0) {
        m_proposalsManager->payForGas(proposal, slotsToPay);
      } else {
        AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
                                         "Invalid data",
                                         "Enter correct number of slots");
      }
    }
  } else if (buttonThatWasClicked == m_fetchProposalsBtn.get()) {
    m_proposalsManager->fetchProposals();
  } else if (buttonThatWasClicked == m_claimRewardBtn.get()) {
    auto proposal = m_proxyModel->getAt(m_proposalsListBox->getSelectedRow());
    const auto budget = Utils::fromWei(CoinUnit::AUTO, proposal->getBudget());
    const String rewardMsg = budget + String(" AUTO is available. \nEnter reward of amount to claim");
    AlertWindow w("Claim reward for " + proposal->getTitle() + " proposal",
                  rewardMsg,
                  AlertWindow::QuestionIcon);

    w.addTextEditor("rewardAmount", "", "Reward Amount:", false);
    w.addButton("OK", 1, KeyPress(KeyPress::returnKey, 0, 0));
    w.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey, 0, 0));

    w.getTextEditor("rewardAmount")->setInputRestrictions(8, "0123456789.");

    if (w.runModalLoop() == 1) {
      const auto rewardAmount = w.getTextEditorContents("rewardAmount").getDoubleValue();
      std::cout << "Reward amount: " << rewardAmount << std::endl;
      if (rewardAmount > 0.0) {
        const auto rewardAmountWei = Utils::toWei(CoinUnit::AUTO, w.getTextEditorContents("rewardAmount"));
        m_proposalsManager->claimReward(proposal, rewardAmountWei);
      } else {
        AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
                                         "Invalid data",
                                         "Enter correct amount of reward to claim");
      }
    }
  }
}

void ProposalsPage::comboBoxChanged(ComboBox* comboBoxThatHasChanged) {
  m_proxyModel->setFilter((ProposalFilter) m_filterByStatusComboBox->getSelectedId());
}

void ProposalsPage::updateButtonsForSelectedProposal(Proposal::Ptr selectedProposal) {
  const bool isRowSelected = m_proposalsListBox->getNumSelectedRows() > 0;
  m_payForGasBtn->setEnabled(isRowSelected);
  m_abandonProposalBtn->setEnabled(isRowSelected);
  m_voteYesBtn->setEnabled(isRowSelected);
  m_voteNoBtn->setEnabled(isRowSelected);

  // TODO(Kirill) need to think which statuses are eligible for claiming (perhaps, Accepted, Completed as well)
  const bool isClaimingActive = selectedProposal->getStatus() == Proposal::Status::Started;
  if (isClaimingActive)
    m_claimRewardBtn->setTooltip("Claim your reward now!");
  else
    m_claimRewardBtn->setTooltip("Proposal claiming is unavailable. Check proposal status, please");
  m_claimRewardBtn->setEnabled(isClaimingActive);
}

// TableListBoxModel
//==============================================================================
int ProposalsPage::getNumRows() {
  return m_proxyModel != nullptr ? m_proxyModel->size() : 0;
}

void ProposalsPage::sortOrderChanged(int columnId, bool isForwards) {
  const int direction = isForwards ? -1 : 1;
  std::function<int(Proposal*, Proposal*)> sorter;

  switch (columnId) {
    case ID: {
      sorter = [=](Proposal* p1, Proposal* p2) {
        return direction * DefaultElementComparator<uint64>::compareElements(p1->getId(), p2->getId());
      };
      break;
    }
    case CreatorAndTitle: {
      sorter = [=](Proposal* p1, Proposal* p2) {
        return direction * DefaultElementComparator<String>::compareElements(p1->getTitle(), p2->getTitle());
      };
      break;
    }
    case ApprovalRating: {
      sorter = [=](Proposal* p1, Proposal* p2) {
        return direction
                * DefaultElementComparator<int>::compareElements(p1->getApprovalRating(), p2->getApprovalRating());
      };
      break;
    }
    case Status: {
      sorter = [=](Proposal* p1, Proposal* p2) {
        return direction
                * DefaultElementComparator<int>
                  ::compareElements(static_cast<int>(p1->getStatus()), static_cast<int>(p2->getStatus()));
      };
      break;
    }
    case Spent: {
      sorter = [=](Proposal* p1, Proposal* p2) {
        return direction
                * DefaultElementComparator<String>::compareElements(p1->getAmountSpent(), p2->getAmountSpent());
      };
      break;
    }
    case Budget: {
      sorter = [=](Proposal* p1, Proposal* p2) {
        return direction
                * DefaultElementComparator<String>::compareElements(p1->getBudget(), p2->getBudget());
      };
      break;
    }
    case Periods: {
      sorter = [=](Proposal* p1, Proposal* p2) {
        return direction * DefaultElementComparator<uint64>::compareElements(p1->getNumPeriods(), p2->getNumPeriods());
      };
      break;
    }
    case Length: {
      sorter = [=](Proposal* p1, Proposal* p2) {
        return direction
                * DefaultElementComparator<String>::compareElements(p1->getAmountSpent(), p2->getAmountSpent());
      };
      break;
    }
    case Bonus: {
      sorter = [=](Proposal* p1, Proposal* p2) {
        return direction
                * DefaultElementComparator<String>::compareElements(p1->getTargetBonus(), p2->getTargetBonus());
      };
      break;
    }
    case TimeLeft: {
      sorter = [=](Proposal* p1, Proposal* p2) {
        return direction * DefaultElementComparator<int>::compareElements(p1->getTimeLeftDays(), p2->getTimeLeftDays());
      };
      break;
    }
    default:
      break;
  }

  m_proxyModel->setSorter(sorter);
}

void ProposalsPage::paintCell(Graphics& g,
                              int rowNumber, int columnId,
                              int width, int height,
                              bool rowIsSelected) {
  auto item = m_proxyModel->getAt(rowNumber);
  if (!item)
      return;

  g.setColour(Colours::white);

  auto getPercentStr = [](float percent) -> String { return String(percent * 100) + "%"; };

  switch (columnId) {
    case ID: {
      g.drawText(String(item->getId()), 0, 0, width, height, Justification::centred);
      break;
    }
    case CreatorAndTitle: {
      g.drawText(String(item->getTitle()), 0, height / 2, width, height / 2, Justification::centredLeft);
      g.setFont(g.getCurrentFont().boldened());
      const auto creatorAlias = item->getCreatorAlias();
      const auto creatorText = creatorAlias.isEmpty()
          ? item->getCreator()
          : creatorAlias + String(" (") + item->getCreator() + String(")");
      g.drawText(creatorText, 0, 0, width, height / 2, Justification::centredLeft);
      break;
    }
    case ApprovalRating: {
      if (item->getApprovalRating() < 0)
        g.setColour(Colours::red);
      else
        g.setColour(Colours::green);

      g.drawText(String(item->getApprovalRating()) + "%", 0, 0, width, height, Justification::centred);
      break;
    }
    case Status: {
      const auto status = item->getStatus();
      const auto areAllSlotsPaid = item->areAllSlotsPaid();
      const auto numSlots = m_accountData->getContractData()->getSlotsNumber();
      if (!areAllSlotsPaid) {
        g.drawText(Proposal::getStatusStr(Proposal::Status::PrepayingGas),
                   0, 0, width, height / 2, Justification::centred);
        g.drawText(String(item->getNumSlotsPaid()) + String("/") + String(numSlots) + String(" paid"),
                   0, height / 2, width, height / 2, Justification::centred);
      } else {
        g.drawText(Proposal::getStatusStr(item->getStatus()), 0, 0, width, height, Justification::centred);
      }
      break;
    }
    case Spent: {
      const String text = item->getAmountSpent() + "(" + getPercentStr(item->getSpentPrecent()) + ")";
      g.drawText(text, 0, 0, width, height, Justification::centred);
      break;
    }
    case Budget: {
      g.drawText(Utils::fromWei(CoinUnit::AUTO, item->getBudget()) + String(" AUTO"),
                 0, 0, width, height, Justification::centred);
      break;
    }
    case Periods: {
      g.drawText(String(item->getNumPeriods()), 0, 0, width, height, Justification::centred);
      break;
    }
    case Length: {
      const auto lengthDays = item->getLengthDays();
      // If length is zero - the proposal has no time limit
      if (lengthDays) {
        const String unit(lengthDays == 1 ? "day" : "days");
        g.drawText(String(lengthDays) + " " + unit, 0, 0, width, height, Justification::centred);
      }
      break;
    }
    case Bonus: {
      const String text = Utils::fromWei(CoinUnit::AUTO, item->getTargetBonus()) + String(" ETH ")
          + "(" + getPercentStr(item->getBounusPrecent()) + ")";
      g.drawText(text, 0, 0, width, height, Justification::centred);
      break;
    }
    case TimeLeft: {
      const String unit(item->getTimeLeftDays() == 1 ? "day" : "days");
      g.drawText(String(item->getTimeLeftDays()) + " " + unit, 0, 0, width, height, Justification::centred);
      break;
    }
    default:
      break;
  }
}

void ProposalsPage::paintRowBackground(Graphics& g,
                                       int rowNumber,
                                       int width, int height,
                                       bool rowIsSelected) {
  auto colour = LookAndFeel::getDefaultLookAndFeel().findColour(TableListBox::backgroundColourId);
  g.setColour(rowIsSelected ? colour.darker(0.3f) : colour);
  g.fillRect(0, 0, width, height);
}

void ProposalsPage::selectedRowsChanged(int lastRowSelected) {
  auto selectedItem = m_proxyModel->getAt(lastRowSelected);
  if (!selectedItem)
      return;
  updateButtonsForSelectedProposal(selectedItem);
}
