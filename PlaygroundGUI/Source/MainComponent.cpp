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

#include "MainComponent.h"

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


DemosMainComponent::DemosMainComponent(Config* config) : m_config(config) {
  m_proposalsManager = std::make_unique<ProposalsManager>(m_config);

  m_tabbedComponent.reset(new TabbedComponent(TabbedButtonBar::TabsAtTop));
  addAndMakeVisible(m_tabbedComponent.get());
  m_tabbedComponent->setTabBarDepth(37);
  m_tabbedComponent->addTab(TRANS("Network"), Colour(0xff404040),
                            new NetworkView(m_proposalsManager.get()), true);
  m_tabbedComponent->addTab(TRANS("Miner"), Colour(0xff404040), new Miner(), true);
  // m_tabbedComponent->addTab(TRANS("Demo Miner"), Colour(0xff404040), new DemoMiner(), true);

  auto proposalsPage = new ProposalsPage(m_proposalsManager.get());
  m_tabbedComponent->addTab(TRANS("Proposals"), Colour(0xff404040), proposalsPage, true);
  m_tabbedComponent->addTab(TRANS("Proposals Actions"), Colour(0xff404040),
                            new ProposalsActionsPage(m_proposalsManager.get()), true);
  m_tabbedComponent->addTab(TRANS("DEX"), Colour(0xff404040), new DEXPage(), true);

  // m_tabbedComponent->addTab(TRANS("Treasury"), Colour(0xff404040), new DemoBlank(), true);
  // m_tabbedComponent->addTab(TRANS("Protocols"), Colour(0xff404040), new DemoBlank(), true);
  // m_tabbedComponent->addTab(TRANS("DApps"), Colour(0xff404040), new DemoBlank(), true);
  // m_tabbedComponent->addTab(TRANS("Demo Grid"), Colour(0xff404040), new DemoGrid(), true);
  // tabbedComponent->addTab(TRANS("Network Simulation"), Colour(0xff404040), new DemoSimNet(), true);
  m_tabbedComponent->setCurrentTabIndex(0);

  setSize(1024, 768);
}

DemosMainComponent::~DemosMainComponent() {
  m_tabbedComponent = nullptr;
}

//==============================================================================
void DemosMainComponent::paint(Graphics& g) {
  g.fillAll(Colour(0xff323e44));
}

void DemosMainComponent::resized() {
  // auto b = getLocalBounds();
  // auto height = LookAndFeel::getDefaultLookAndFeel().getDefaultMenuBarHeight();
  // menuBar->setBounds(b.removeFromTop(height));

  m_tabbedComponent->setBounds(8, 8, getWidth() - 16, getHeight() - 16);
}
