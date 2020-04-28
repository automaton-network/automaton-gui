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
#include <Components/SlotsGrid.h>
#include "ProposalDetailsComponent.h"
#include "../Data/AutomatonContractData.h"
#include "ProposalsManager.h"

class VoteSlotsGrid : public SlotsGrid {
 public:
  VoteSlotsGrid() {
    m_message.setText("Loading........", NotificationType::dontSendNotification);
    m_message.setJustificationType(Justification::centred);
    addAndMakeVisible(m_message);
  }

  void setSlots(const Array<uint64>& slots) {
    m_slots = slots;
    m_message.setVisible(m_slots.size() == 0);
    updateContent();
  }

  Colour getSlotColour(int slotIndex) override {
    switch (m_slots[slotIndex]) {
      case 0: return Colour(0xffbdbdbd);  // light gray
      case 1: return Colour(0xff388e3c);  // Material green
      case 2: return Colour(0xffff5252);  // Material red
      default: return Colours::blue;  // Display blue as an unimplemented slot vote (mostly for debug purposes)
    }
  }

  int getNumOfSlots() override {
    return m_slots.size();
  }

  void resized() {
    m_message.setBounds(getLocalBounds());
  }

 private:
  Label m_message;
  Array<uint64> m_slots;
};

//==============================================================================
ProposalDetailsComponent::ProposalDetailsComponent(Account::Ptr accountData) : m_accountData(accountData) {
  m_slotsGrid = std::make_unique<VoteSlotsGrid>();
  m_linkToDocument = std::make_unique<HyperlinkButton>();
  m_linkToDocument->setJustificationType(Justification::centredLeft);
  m_backBtn = std::make_unique<TextButton>("Back");

  m_backBtn->addListener(this);

  addAndMakeVisible(m_title);
  addAndMakeVisible(m_reward);
  addAndMakeVisible(m_status);
  addAndMakeVisible(m_linkToDocument.get());
  addAndMakeVisible(m_backBtn.get());
  addAndMakeVisible(m_slotsGrid.get());
}

ProposalDetailsComponent::~ProposalDetailsComponent() {
}

void ProposalDetailsComponent::setProposal(Proposal::Ptr proposal) {
  if (!proposal) {
    setVisible(false);
    return;
  }

  m_proposal = proposal;
  m_proposal->addListener(this);
  m_title.setText("Proposal " + proposal->getTitle(), NotificationType::dontSendNotification);
  m_reward.setText(Utils::fromWei(CoinUnit::AUTO,
      proposal->getBudgetPerPeriod()) + String(" AUTO"), NotificationType::dontSendNotification);

  String statusStr;
  if (!proposal->areAllSlotsPaid()) {
    const auto numSlots = m_accountData->getContractData()->getSlotsNumber();
    statusStr = Proposal::getStatusStr(Proposal::Status::PrepayingGas) + " ("
        + String(proposal->getNumSlotsPaid()) + String("/") + String(numSlots) + String(") paid");
  } else {
    statusStr = Proposal::getStatusStr(proposal->getStatus());
  }

  m_status.setText(statusStr, NotificationType::dontSendNotification);
  m_linkToDocument->setButtonText(proposal->getDocumentLink());
  m_linkToDocument->setURL(URL(proposal->getDocumentLink()));
  m_slotsGrid->setSlots(m_proposal->getSlots());
  m_accountData->getProposalsManager()->fetchProposalVotes(m_proposal);
}

void ProposalDetailsComponent::paint(Graphics& g) {
  g.setColour(Colour(0xff404040));
  g.fillRect(getLocalBounds());
}

void ProposalDetailsComponent::resized() {
  auto bounds = getLocalBounds().removeFromLeft(getWidth() / 2);
  m_title.setBounds(bounds.removeFromTop(30));
  m_reward.setBounds(bounds.removeFromTop(30));
  m_status.setBounds(bounds.removeFromTop(30));
  m_linkToDocument->setBounds(bounds.removeFromTop(30));
  m_backBtn->setBounds(bounds.removeFromBottom(30).removeFromLeft(100));

  auto gridBounds = getLocalBounds().removeFromRight(getWidth() / 2);
  m_slotsGrid->setBounds(gridBounds);
}

void ProposalDetailsComponent::buttonClicked(Button* button) {
  if (m_backBtn.get() == button) {
    m_proposal->removeListener(this);
    setVisible(false);
  }
}
void ProposalDetailsComponent::proposalChanged() {
  triggerAsyncUpdate();
}

void ProposalDetailsComponent::handleAsyncUpdate() {
  m_slotsGrid->setSlots(m_proposal->getSlots());
}
