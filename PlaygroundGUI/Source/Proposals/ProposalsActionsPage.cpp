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
#include "ProposalsActionsPage.h"
#include "GridView.h"
#include "ProposalsManager.h"

class ProposalComponent : public Component
                        , private Button::Listener {
 public:
  class Listener {
   public:
    virtual ~Listener() = default;
    virtual void castVote(Proposal::Ptr proposal, uint64 choice) = 0;
  };

  ProposalComponent() {
    m_title = std::make_unique<Label>();
    m_title->setFont(m_title->getFont().withHeight(25));
    m_linkToDocBtn = std::make_unique<HyperlinkButton>(translate("Link to documentation"), URL());
    m_approveBtn = std::make_unique<TextButton>(translate("Approve"));
    m_rejectBtn = std::make_unique<TextButton>(translate("Reject"));

    m_approveBtn->addListener(this);
    m_rejectBtn->addListener(this);

    addAndMakeVisible(m_title.get());
    addAndMakeVisible(m_linkToDocBtn.get());
    addAndMakeVisible(m_approveBtn.get());
    addAndMakeVisible(m_rejectBtn.get());
  }

  void setData(Proposal::Ptr proposal) {
    m_proposal = proposal;
    m_title->setText(m_proposal->getTitle(), NotificationType::dontSendNotification);
    m_proposalState = Proposal::getStatusStr(m_proposal->getStatus()) + " Proposal";
  }

  void resized() override {
    auto bounds = getLocalBounds().reduced(10, 30);
    m_title->setBounds(bounds.removeFromTop(25));
    auto buttonsBound = bounds.removeFromBottom(25).reduced(10, 0);
    m_approveBtn->setBounds(buttonsBound.removeFromLeft(100));
    buttonsBound.removeFromLeft(10);
    m_rejectBtn->setBounds(buttonsBound.removeFromLeft(100));
  }

  void paint(Graphics& g) override {
    g.setColour(Colours::white);
    g.setFont(g.getCurrentFont().withHeight(18));
    const int stateStrWidth = g.getCurrentFont().getStringWidth(m_proposalState) + 10;

    const auto bounds = getLocalBounds().withTrimmedTop(10).toFloat();
    const float topLeftLineWidth = bounds.getWidth() * 0.1f;
    Path p;
    p.startNewSubPath(bounds.getTopLeft());
    p.lineTo(bounds.getBottomLeft());
    p.lineTo(bounds.getBottomRight());
    p.lineTo(bounds.getTopRight());
    p.lineTo(topLeftLineWidth + stateStrWidth, bounds.getY());
    p.startNewSubPath(bounds.getTopLeft());
    p.lineTo(topLeftLineWidth, bounds.getY());
    g.strokePath(p, PathStrokeType(2.0f));

    g.drawText(m_proposalState, static_cast<int>(topLeftLineWidth) + 5, 0, stateStrWidth, 12, Justification::topLeft);
  }

  ListenerList<Listener> m_listeners;

 private:
  void buttonClicked(Button* buttonThatWasClicked) override {
    // Might be better to move that responsibility to top layers in the future (when more vote choices are added)
    if (buttonThatWasClicked == m_approveBtn.get()) {
      m_listeners.call(&Listener::castVote, m_proposal, 1);
    } else if (buttonThatWasClicked == m_rejectBtn.get()) {
      m_listeners.call(&Listener::castVote, m_proposal, 2);
    }
  }

 private:
  String m_proposalState;
  TextLayout m_text;
  Proposal::Ptr m_proposal;

  std::unique_ptr<Label> m_title;
  std::unique_ptr<HyperlinkButton> m_linkToDocBtn;
  std::unique_ptr<TextButton> m_approveBtn;
  std::unique_ptr<TextButton> m_rejectBtn;
};

class ProposalsGrid : public GridView
                    , public AbstractListModelBase::Listener
                    , public ProposalComponent::Listener {
 public:
  ProposalsGrid(ProposalsManager* proposalsManager) : m_proposalsManager(proposalsManager) {
  }

  void castVote(Proposal::Ptr proposal, uint64 choice) {
    m_proposalsManager->castVote(proposal, choice);
  }

  void setModel(std::shared_ptr<AbstractListModel<Proposal::Ptr>> model) {
    m_model = model;
    m_model->addListener(this);
    updateContent();
  }

  void modelChanged(AbstractListModelBase*) override {
    updateContent();
    resized();
  }

  Component* refreshComponent(int index, Component* const componentToUpdate) override {
    auto comp = dynamic_cast<ProposalComponent*>(componentToUpdate);
    if (comp == nullptr)
      comp = new ProposalComponent();

    comp->m_listeners.add(this);
    comp->setData(m_model->getAt(index));
    return comp;
  }

  int size() const override {
    return m_model->size();
  }

 private:
  std::shared_ptr<AbstractListModel<Proposal::Ptr>> m_model;
  ProposalsManager* m_proposalsManager;
};

//==============================================================================
ProposalsActionsPage::ProposalsActionsPage(ProposalsManager* proposalsManager) : m_proposalsManager(proposalsManager) {
  m_titleLabel = std::make_unique<Label>("m_titleLabel", translate("Validator Actions Required:"));
  m_titleLabel->setColour(Label::textColourId, Colours::white);
  m_titleLabel->setFont(m_titleLabel->getFont().withHeight(45));
  addAndMakeVisible(m_titleLabel.get());

  m_proposalsGrid = std::make_unique<ProposalsGrid>(proposalsManager);
  m_proposalsGrid->setModel(m_proposalsManager->getModel());
  m_proposalsGrid->setMargins(50, 25, 50, 50);
  m_proposalsGrid->setCellMinimumWidth(350);
  m_proposalsGrid->setCellRatio(1.75);
  addAndMakeVisible(m_proposalsGrid.get());
}

ProposalsActionsPage::~ProposalsActionsPage() {
}

void ProposalsActionsPage::paint(Graphics& g) {
}

void ProposalsActionsPage::resized() {
  auto bounds = getLocalBounds();
  bounds.removeFromTop(25);
  m_titleLabel->setBounds(bounds.removeFromTop(50).withTrimmedLeft(50));
  m_proposalsGrid->setBounds(bounds);
}
