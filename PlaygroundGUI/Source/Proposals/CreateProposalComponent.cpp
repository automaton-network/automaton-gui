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

#include "CreateProposalComponent.h"
#include "../Utils/Utils.h"

CreateProposalComponent::CreateProposalComponent() {
  m_titleEditor = std::make_unique<TextEditor>(translate("Title"));
  addAndMakeVisible(m_titleEditor.get());

  m_budgetEditor = std::make_unique<TextEditor>(translate("Budget (AUTO)"));
  m_budgetEditor->setInputRestrictions(8, Utils::numericalFloatAllowed);
  m_budgetEditor->addListener(this);
  addAndMakeVisible(m_budgetEditor.get());

  m_numPeriodsEditor = std::make_unique<TextEditor>(translate("Num periods"));
  m_numPeriodsEditor->setInputRestrictions(5, Utils::numericalIntegerAllowed);
  m_numPeriodsEditor->addListener(this);
  addAndMakeVisible(m_numPeriodsEditor.get());

  m_totalBudgetEditor = std::make_unique<TextEditor>(translate("Total Budget (AUTO)"));
  m_totalBudgetEditor->setInputRestrictions(8, Utils::numericalFloatAllowed);
  m_totalBudgetEditor->setReadOnly(true);
  addAndMakeVisible(m_totalBudgetEditor.get());

  m_targetBonusEditor = std::make_unique<TextEditor>(translate("Target bonus (AUTO)"));
  m_targetBonusEditor->setInputRestrictions(8, Utils::numericalFloatAllowed);
  // addAndMakeVisible(m_targetBonusEditor.get());

  m_lengthDaysEditor = std::make_unique<TextEditor>(translate("Length (days)"));
  m_lengthDaysEditor->setInputRestrictions(5, Utils::numericalIntegerAllowed);
  addAndMakeVisible(m_lengthDaysEditor.get());

  m_createProposalBtn = std::make_unique<TextButton>(translate("Create proposal"));
  m_createProposalBtn->addListener(this);
  addAndMakeVisible(m_createProposalBtn.get());

  m_cancelBtn = std::make_unique<TextButton>(translate("Cancel"));
  m_cancelBtn->addListener(this);
  addAndMakeVisible(m_cancelBtn.get());
}

CreateProposalComponent::~CreateProposalComponent() {
  if (m_proposal != nullptr)
    m_proposal->removeListener(this);
}

void CreateProposalComponent::paint(Graphics& g) {
  g.setColour(Colour(0xff404040));
  g.fillRect(getLocalBounds());

  const auto spacing = 20;
  const auto textWidth = 130;
  const auto componentHeight = 30;
  auto bounds = getLocalBounds().reduced(spacing);
  bounds = bounds.removeFromLeft(textWidth);

  g.setColour(Colours::white);
  g.drawText(m_titleEditor->getName(), bounds.removeFromTop(componentHeight), Justification::centredLeft);
  bounds.removeFromTop(spacing);

  g.drawText(m_budgetEditor->getName(), bounds.removeFromTop(componentHeight), Justification::centredLeft);
  bounds.removeFromTop(spacing);

  g.drawText(m_numPeriodsEditor->getName(), bounds.removeFromTop(componentHeight), Justification::centredLeft);
  bounds.removeFromTop(spacing);

  g.drawText(m_totalBudgetEditor->getName(), bounds.removeFromTop(componentHeight), Justification::centredLeft);
  bounds.removeFromTop(spacing);

  // g.drawText(m_targetBonusEditor->getName(), bounds.removeFromTop(componentHeight), Justification::centredLeft);
  // bounds.removeFromTop(spacing);

  g.drawText(m_lengthDaysEditor->getName(), bounds.removeFromTop(componentHeight), Justification::centredLeft);
  bounds.removeFromTop(spacing);
}

void CreateProposalComponent::resized() {
  const auto spacing = 20;
  const auto textWidth = 130;
  const auto componentHeight = 30;
  auto bounds = getLocalBounds().reduced(spacing);
  bounds.removeFromLeft(textWidth + spacing);

  m_titleEditor->setBounds(bounds.removeFromTop(componentHeight));
  bounds.removeFromTop(spacing);

  m_budgetEditor->setBounds(bounds.removeFromTop(componentHeight));
  bounds.removeFromTop(spacing);

  m_numPeriodsEditor->setBounds(bounds.removeFromTop(componentHeight));
  bounds.removeFromTop(spacing);

  m_totalBudgetEditor->setBounds(bounds.removeFromTop(componentHeight));
  bounds.removeFromTop(spacing);

  // m_targetBonusEditor->setBounds(bounds.removeFromTop(componentHeight));
  // bounds.removeFromTop(spacing);

  m_lengthDaysEditor->setBounds(bounds.removeFromTop(componentHeight));
  bounds.removeFromTop(spacing);

  bounds = bounds.removeFromTop(componentHeight);
  m_cancelBtn->setBounds(bounds.removeFromLeft(bounds.getWidth() / 2));
  bounds.removeFromLeft(spacing);
  m_createProposalBtn->setBounds(bounds);
}

void CreateProposalComponent::buttonClicked(Button* buttonThatWasClicked) {
  if (buttonThatWasClicked == m_createProposalBtn.get()) {
    auto checkEmptiness = [](TextEditor* t, const String& warningMessage) -> bool {
      const auto text = t->getText();
      if (text.isEmpty()) {
        AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
                                         "Invalid data",
                                         warningMessage);
        return false;
      }
      return true;
    };

    if (!checkEmptiness(m_titleEditor.get(), String("Enter proposal title, please"))) return;
    if (!checkEmptiness(m_budgetEditor.get(), String("Enter proposal budget, please"))) return;
    if (!checkEmptiness(m_numPeriodsEditor.get(), String("Enter number of periods for proposal, please"))) return;
    // if (!checkEmptiness(m_targetBonusEditor.get(), String("Enter target bonus for proposal, please"))) return;
    // Length days could be empty

    if (m_proposal != nullptr)
      m_proposal->removeListener(this);

    m_proposal = std::make_shared<Proposal>();
    m_proposal->setTitle(m_titleEditor->getText());
    m_proposal->setBudgetPerPeriod(Utils::toWei(CoinUnit::AUTO, m_budgetEditor->getText()));
    m_proposal->setNumPeriodsLeft(static_cast<uint64>(m_numPeriodsEditor->getText().getLargeIntValue()));
    // m_proposal->setTargetBonus(Utils::toWei(CoinUnit::AUTO, m_targetBonusEditor->getText()));
    if (!m_lengthDaysEditor->isEmpty())
      m_proposal->setBudgetPeriodLength(static_cast<uint64>(m_lengthDaysEditor->getText().getLargeIntValue()));

    m_proposal->addListener(this);
    m_listeners.call(&CreateProposalComponent::Listener::createProposalViewActionHappened,
                     this,
                     Action::ProposalCreated);
  } else if (buttonThatWasClicked == m_cancelBtn.get()) {
    m_listeners.call(&CreateProposalComponent::Listener::createProposalViewActionHappened, this, Action::Cancelled);
  }
}

void CreateProposalComponent::textEditorTextChanged(TextEditor& editor) {
  if (&editor == m_budgetEditor.get() || &editor == m_numPeriodsEditor.get()) {
    if (m_budgetEditor->getText().isEmpty() || m_numPeriodsEditor->getText().isEmpty())
      return;

    const auto numPeriods = static_cast<uint64>(m_numPeriodsEditor->getText().getLargeIntValue());
    const auto budgetPerPeriod = static_cast<uint64>(m_budgetEditor->getText().getLargeIntValue());
    m_totalBudgetEditor->setText(String(numPeriods * budgetPerPeriod));
  }
}

void CreateProposalComponent::proposalChanged() {
  if (m_proposal != nullptr && !isVisible() && m_proposal->getStatus() != Proposal::Status::Uninitialized) {
    clearFields();
  }
}

void CreateProposalComponent::clearFields() {
  m_titleEditor->clear();
  m_budgetEditor->clear();
  m_totalBudgetEditor->clear();
  m_numPeriodsEditor->clear();
  m_targetBonusEditor->clear();
  m_lengthDaysEditor->clear();
}
