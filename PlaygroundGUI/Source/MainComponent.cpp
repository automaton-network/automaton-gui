/*
 * Automaton Playground
 * Copyright (c) 2019 The Automaton Authors.
 * Copyright (c) 2019 The automaton.network Authors.
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

#include "Demos/DemoGrid.h"
#include "Demos/DemoMiner.h"
#include "Demos/DemoSimNet.h"
#include "Miner/Miner.h"
#include "Network/NetworkView.h"
#include "Proposals/ProposalsManager.h"
#include "Proposals/ProposalsPage.h"
#include "Proposals/ProposalsActionsPage.h"
#include "DEX/DEXPage.h"
#include "DEX/DEXManager.h"
#include "MainComponent.h"
#include "Utils/TasksPanel.h"
#include "Data/AutomatonContractData.h"
#include "Debug/DebugPage.h"

class DemoBlank: public Component {
 public:
  DemoBlank() {}
  ~DemoBlank() {}

  void paint(Graphics& g) override {
  }

  void resized() override {
  }

 private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DemoBlank)
};


DemosMainComponent::DemosMainComponent(Account::Ptr accountData) : m_accountData(accountData) {
  accountData->initManagers();

  m_accountData->getProposalsManager()->fetchProposals();
  m_accountData->getDexManager()->fetchOrders();

  m_tabbedComponent.reset(new TabbedComponent(TabbedButtonBar::TabsAtTop));
  addAndMakeVisible(m_tabbedComponent.get());
  m_tabbedComponent->setTabBarDepth(37);
//  m_tabbedComponent->addTab(TRANS("Network"), Colour(0xff404040),
//                            new NetworkView(m_proposalsManager.get()), true);
  m_tabbedComponent->addTab(TRANS("Miner"), Colour(0xff404040), new Miner(m_accountData), true);
  // m_tabbedComponent->addTab(TRANS("Demo Miner"), Colour(0xff404040), new DemoMiner(), true);

  auto proposalsPage = new ProposalsPage(m_accountData);
  m_tabbedComponent->addTab(TRANS("Proposals"), Colour(0xff404040), proposalsPage, true);
  m_tabbedComponent->addTab(TRANS("Proposals Actions"), Colour(0xff404040),
                            new ProposalsActionsPage(m_accountData->getProposalsManager()), true);
  m_tabbedComponent->addTab(TRANS("DEX"), Colour(0xff404040), new DEXPage(m_accountData), true);
  m_tabbedComponent->addTab(TRANS("Debug"), Colour(0xff404040), new DebugPage(), true);

  // m_tabbedComponent->addTab(TRANS("Treasury"), Colour(0xff404040), new DemoBlank(), true);
  // m_tabbedComponent->addTab(TRANS("Protocols"), Colour(0xff404040), new DemoBlank(), true);
  // m_tabbedComponent->addTab(TRANS("DApps"), Colour(0xff404040), new DemoBlank(), true);
  // m_tabbedComponent->addTab(TRANS("Demo Grid"), Colour(0xff404040), new DemoGrid(), true);
  // tabbedComponent->addTab(TRANS("Network Simulation"), Colour(0xff404040), new DemoSimNet(), true);
  m_tabbedComponent->setCurrentTabIndex(0);

  m_tasksPanel = std::make_unique<TasksPanel>(accountData);
  addAndMakeVisible(m_tasksPanel->getStatusBarComponent());
  addChildComponent(m_tasksPanel.get());

  m_refreshButton = std::make_unique<TextButton>("Refresh");
  m_refreshButton->addListener(this);
  addAndMakeVisible(m_refreshButton.get());

  setSize(1024, 768);
}

DemosMainComponent::~DemosMainComponent() {
  m_accountData->clearManagers();
  m_tabbedComponent = nullptr;
}

//==============================================================================
void DemosMainComponent::paint(Graphics& g) {
  g.fillAll(Colour(0xff323e44));
}

void DemosMainComponent::resized() {
  auto bounds = getLocalBounds();
  const auto statusBarBounds = bounds.removeFromBottom(25);

  m_refreshButton->setBounds(getLocalBounds().reduced(10).removeFromRight(80).removeFromTop(25));
  m_tasksPanel->setBounds(bounds.withWidth(300));
  m_tabbedComponent->setBounds(8, 8,
                               bounds.getWidth() - 16, getLocalBounds().getHeight() - 8 - statusBarBounds.getHeight());
  m_tasksPanel->getStatusBarComponent()->setBounds(statusBarBounds);
}

void DemosMainComponent::buttonClicked(Button* button) {
  if (m_refreshButton.get() == button) {
    m_accountData->getContractData()->readContract();
    m_accountData->getProposalsManager()->fetchProposals();
    m_accountData->getDexManager()->fetchOrders();
  }
}
