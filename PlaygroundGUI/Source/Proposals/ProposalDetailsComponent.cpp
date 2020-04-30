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
#include "automaton/core/io/io.h"

using automaton::core::io::bin2hex;

class VoteSlotsGrid : public SlotsGrid {
 public:
  VoteSlotsGrid() {
    m_message.setText("Loading........", NotificationType::dontSendNotification);
    m_message.setJustificationType(Justification::centred);
    addAndMakeVisible(m_message);
  }

  void setSlots(const Array<uint64>& slots, const std::vector<ValidatorSlot>& validatorSlots) {
    m_slots = slots;
    m_validatorSlots = validatorSlots;
    m_message.setVisible(m_slots.size() == 0);
    updateContent();
  }

  Colour getSlotColour(int slotIndex, bool isHighlighted) override {
    Colour slotColour;
    switch (m_slots[slotIndex]) {
      case 0: slotColour = Colour(0xffbdbdbd); break;  // light gray
      case 1: slotColour = Colour(0xff388e3c); break;  // Material green
      case 2: slotColour = Colour(0xffff5252); break;  // Material red
      default: slotColour = Colours::blue;  // Display blue as an unimplemented slot vote (mostly for debug purposes)
    }

    if (isHighlighted)
      slotColour = slotColour.contrasting(0.1f);

    return slotColour;
  }

  int getNumOfSlots() override {
    return m_slots.size();
  }

  Component* getPopupComponent(int slotIndex) override {
    if (slotIndex < 0)
      return nullptr;

    const auto vote = m_slots[slotIndex];
    String slotInfo;
    slotInfo << "Slot: " << slotIndex << "\n" <<
                "Vote: " << (vote == 1 ? "YES" : vote == 2 ? "NO" : "Unspecified") << "\n" <<
                "Owner: " << m_validatorSlots[slotIndex].owner << "\n" <<
                "Difficulty:" << bin2hex(m_validatorSlots[slotIndex].difficulty) << "\n";

    m_popup.m_label.setText(slotInfo, NotificationType::dontSendNotification);
    m_popup.setSize(450, 100);
    m_popup.setVisible(true);
    return &m_popup;
  }

  void resized() {
    m_message.setBounds(getLocalBounds());
  }

 private:
  Label m_message;
  Array<uint64> m_slots;
  std::vector<ValidatorSlot> m_validatorSlots;

  class SlotPopup : public Component {
   public:
    SlotPopup() {
      addAndMakeVisible(m_label);
    }
    void resized() override {
      m_label.setBounds(getLocalBounds());
    }
    void paint(Graphics& g) override {
      g.fillAll(Colour(0xff404040));
    }

    Label m_label;
  } m_popup;
};

//==============================================================================
ProposalDetailsComponent::ProposalDetailsComponent(Account::Ptr accountData) : m_accountData(accountData) {
  m_slotsGrid = std::make_unique<VoteSlotsGrid>();
  m_linkToDocument = std::make_unique<HyperlinkButton>();
  m_linkToDocument->setJustificationType(Justification::centredLeft);
  m_backBtn = std::make_unique<TextButton>("Back");

  m_backBtn->addListener(this);

  m_title.setFont(m_title.getFont().withHeight(35));

  m_voteYesBtn = std::make_unique<TextButton>(translate("Vote YES"));
  m_voteYesBtn->addListener(this);
  m_voteNoBtn = std::make_unique<TextButton>(translate("Vote NO"));
  m_voteNoBtn->addListener(this);
  m_unspecifiedBtn = std::make_unique<TextButton>(translate("Vote Unspecified"));
  m_unspecifiedBtn->addListener(this);
  m_claimRewardBtn = std::make_unique<TextButton>(translate("Claim reward"));
  m_claimRewardBtn->addListener(this);

  addAndMakeVisible(m_title);
  addAndMakeVisible(m_proposalDetailsLabel);
  addAndMakeVisible(m_linkToDocument.get());
  addAndMakeVisible(m_backBtn.get());
  addAndMakeVisible(m_slotsGrid.get());
  addAndMakeVisible(m_voteYesBtn.get());
  addAndMakeVisible(m_voteNoBtn.get());
  addAndMakeVisible(m_unspecifiedBtn.get());
  addAndMakeVisible(m_claimRewardBtn.get());
}

ProposalDetailsComponent::~ProposalDetailsComponent() {
}

static String formatStatusString(Proposal::Ptr proposal, Account::Ptr accountData) {
  const auto status = proposal->getStatus();
  const auto areAllSlotsPaid = proposal->areAllSlotsPaid();
  const auto numSlots = accountData->getContractData()->getSlotsNumber();
  String result;
  if (!areAllSlotsPaid) {
    result << Proposal::getStatusStr(Proposal::Status::PrepayingGas) << " "
    << String(proposal->getNumSlotsPaid()) + String("/") + String(numSlots) + String(" paid");
  } else if (status == Proposal::Status::Started) {  // Voting state
    const auto initialVotingEndDate = proposal->getInitialVotingEndDate();
    if (Utils::isZeroTime(initialVotingEndDate) == false) {
      const auto votingEnded = initialVotingEndDate.toMilliseconds() < Time::getCurrentTime().toMilliseconds();
      const auto initialVotingEndDateMsg = (votingEnded ? "Ended at " : "Ends at ")
          + initialVotingEndDate.toString(true, true, true, true);
      result << Proposal::getStatusStr(proposal->getStatus()) << " " << initialVotingEndDateMsg;
    } else {
      result << "Started";
    }
  } else {
    result << Proposal::getStatusStr(proposal->getStatus());
  }

  return result;
}

static String formatClaimString(Proposal::Ptr proposal) {
  String result;

  const auto nextPaymentDate = proposal->getNextPaymentDate();
  if (proposal->isRewardClaimable()
      && Utils::isZeroTime(nextPaymentDate) == false) {
    const auto claimableInMs = nextPaymentDate.toMilliseconds() - Time::getCurrentTime().toMilliseconds();
    String claimText;
    if (claimableInMs >= 0) {
      const RelativeTime claimableInRelativeTime(claimableInMs / 1000);
      claimText = "Claimable in " + claimableInRelativeTime.getDescription();
    } else {
      claimText = "Claim now!";
    }
    result << claimText;
  }

  return result;
}

void ProposalDetailsComponent::setProposal(Proposal::Ptr proposal) {
  if (!proposal) {
    setVisible(false);
    return;
  }

  m_proposal = proposal;
  m_proposal->addListener(this);
  m_title.setText("Proposal " + proposal->getTitle(), NotificationType::dontSendNotification);
  const String rewardStr = "Reward per period: " + Utils::fromWei(CoinUnit::AUTO, proposal->getBudgetPerPeriod()) + String(" AUTO");
  const String periodsStr = "Periods left: " + String(m_proposal->getNumPeriodsLeft());

  m_proposalDetailsLabel.setText(rewardStr + "\n\n" + periodsStr + "\n\n" + formatStatusString(m_proposal, m_accountData) +
                                 "\n\n" + formatClaimString(m_proposal), NotificationType::dontSendNotification);

  m_linkToDocument->setButtonText(proposal->getDocumentLink());
  m_linkToDocument->setURL(URL(proposal->getDocumentLink()));
  m_slotsGrid->setSlots(m_proposal->getSlots(), m_accountData->getContractData()->getSlots());
  m_accountData->getProposalsManager()->fetchProposalVotes(m_proposal);
  updateButtonsForProposal();
}

void ProposalDetailsComponent::updateButtonsForProposal() {
  const auto proposalStatus = m_proposal->getStatus();

  const bool isClaimingActive = m_proposal->isRewardClaimable();
  if (isClaimingActive)
    m_claimRewardBtn->setTooltip("Claim your reward now!");
  else
    m_claimRewardBtn->setTooltip("Proposal claiming is unavailable. Check proposal status, please");

  m_claimRewardBtn->setEnabled(isClaimingActive);
}

void ProposalDetailsComponent::paint(Graphics& g) {
  g.setColour(Colour(0xff404040));
  g.fillRect(getLocalBounds());
}

void ProposalDetailsComponent::resized() {
  auto bounds = getLocalBounds().removeFromLeft(getWidth() / 2);
  m_title.setBounds(bounds.removeFromTop(40));
  m_proposalDetailsLabel.setBounds(bounds.removeFromTop(100));
  m_linkToDocument->setBounds(bounds.removeFromTop(30));
  m_backBtn->setBounds(bounds.removeFromBottom(30).removeFromLeft(100));

  bounds.removeFromTop(30);
  auto buttonsBounds = bounds.removeFromTop(30);
  m_voteYesBtn->setBounds(buttonsBounds.removeFromLeft(75));
  buttonsBounds.removeFromLeft(5);
  m_voteNoBtn->setBounds(buttonsBounds.removeFromLeft(75));
  buttonsBounds.removeFromLeft(5);
  m_unspecifiedBtn->setBounds(buttonsBounds.removeFromLeft(100));
  buttonsBounds.removeFromLeft(5);
  m_claimRewardBtn->setBounds(buttonsBounds.removeFromLeft(100));

  auto gridBounds = getLocalBounds().removeFromRight(getWidth() / 2);
  m_slotsGrid->setBounds(gridBounds);
}

void ProposalDetailsComponent::buttonClicked(Button* button) {
  if (m_backBtn.get() == button) {
    m_proposal->removeListener(this);
    setVisible(false);
  } else if (button == m_voteYesBtn.get()) {
    m_accountData->getProposalsManager()->castVote(m_proposal, 1);
  } else if (button == m_voteNoBtn.get()) {
    m_accountData->getProposalsManager()->castVote(m_proposal, 2);
  }
  else if (button == m_unspecifiedBtn.get()) {
    m_accountData->getProposalsManager()->castVote(m_proposal, 0);
  }
  else if (button == m_claimRewardBtn.get()) {
    const auto budget = Utils::fromWei(CoinUnit::AUTO, m_proposal->getBudgetPerPeriod());
    const String rewardMsg = budget + String(" AUTO is available. \nEnter reward of amount to claim");
    AlertWindow w("Claim reward for " + m_proposal->getTitle() + " proposal",
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
        m_accountData->getProposalsManager()->claimReward(m_proposal, rewardAmountWei);
      } else {
        AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
                                         "Invalid data",
                                         "Enter correct amount of reward to claim");
      }
    }
  }
}
void ProposalDetailsComponent::proposalChanged() {
  triggerAsyncUpdate();
}

void ProposalDetailsComponent::handleAsyncUpdate() {
  m_slotsGrid->setSlots(m_proposal->getSlots(), m_accountData->getContractData()->getSlots());
}
