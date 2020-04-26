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

#include <JuceHeader.h>
#include <Utils/Utils.h>
#include "ProposalDetailsComponent.h"
#include "../Data/AutomatonContractData.h"

//==============================================================================
ProposalDetailsComponent::ProposalDetailsComponent(Account::Ptr accountData) : m_accountData(accountData) {
  m_linkToDocument = std::make_unique<HyperlinkButton>();
  m_linkToDocument->setJustificationType(Justification::centredLeft);
  m_backBtn = std::make_unique<TextButton>("Back");

  m_backBtn->addListener(this);

  addAndMakeVisible(m_title);
  addAndMakeVisible(m_reward);
  addAndMakeVisible(m_status);
  addAndMakeVisible(m_linkToDocument.get());
  addAndMakeVisible(m_backBtn.get());
}

ProposalDetailsComponent::~ProposalDetailsComponent() {
}

void ProposalDetailsComponent::setProposal(Proposal::Ptr proposal) {
  m_proposal = proposal;
  m_title.setText("Proposal " + proposal->getTitle(), NotificationType::dontSendNotification);
  m_reward.setText(Utils::fromWei(CoinUnit::AUTO,
      proposal->getBudget()) + String(" AUTO"), NotificationType::dontSendNotification);

  String statusStr;
  if (!proposal->areAllSlotsPaid()) {
    const auto numSlots = m_accountData->getContractData()->getSlotsNumber();
    statusStr = Proposal::getStatusStr(Proposal::Status::PrepayingGas) +
        String(proposal->getNumSlotsPaid()) + String("/") + String(numSlots) + String(" paid");
  } else {
    statusStr = Proposal::getStatusStr(proposal->getStatus());
  }

  m_status.setText(statusStr, NotificationType::dontSendNotification);
  m_linkToDocument->setButtonText(proposal->getDocumentLink());
  m_linkToDocument->setURL(URL(proposal->getDocumentLink()));
}

void ProposalDetailsComponent::paint(Graphics& g) {
  g.setColour(Colour(0xff404040));
  g.fillRect(getLocalBounds());

  auto bounds = getLocalBounds().removeFromLeft(getWidth() / 2);
  m_title.setBounds(bounds.removeFromTop(30));
  m_reward.setBounds(bounds.removeFromTop(30));
  m_status.setBounds(bounds.removeFromTop(30));
  m_linkToDocument->setBounds(bounds.removeFromTop(30));
  m_backBtn->setBounds(bounds.removeFromBottom(30).removeFromLeft(100));
}

void ProposalDetailsComponent::resized() {
}

void ProposalDetailsComponent::buttonClicked(Button* button) {
  if (m_backBtn.get() == button) {
    setVisible(false);
  }
}
