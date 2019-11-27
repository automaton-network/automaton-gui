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


DemosMainComponent::DemosMainComponent() {
  // menuBar.reset(new MenuBarComponent(this));
  // addAndMakeVisible(menuBar.get());

  std::shared_ptr<SlotsModelInterface> model = std::make_shared<SlotsModelTest>();

  tabbedComponent.reset(new TabbedComponent(TabbedButtonBar::TabsAtTop));
  addAndMakeVisible(tabbedComponent.get());
  tabbedComponent->setTabBarDepth(37);
  tabbedComponent->addTab(TRANS("Network"), Colour(0xff404040), new NetworkView(), true);
  tabbedComponent->addTab(TRANS("Miner"), Colour(0xff404040), new Miner(), true);
  tabbedComponent->addTab(TRANS("Demo Miner"), Colour(0xff404040), new DemoMiner(), true);
  tabbedComponent->addTab(TRANS("Treasury"), Colour(0xff404040), new DemoBlank(), true);
  tabbedComponent->addTab(TRANS("Protocols"), Colour(0xff404040), new DemoBlank(), true);
  tabbedComponent->addTab(TRANS("DApps"), Colour(0xff404040), new DemoBlank(), true);
  tabbedComponent->addTab(TRANS("Demo Grid"), Colour(0xff404040), new DemoGrid(model, TEST_OWNER), true);
  // tabbedComponent->addTab(TRANS("Network Simulation"), Colour(0xff404040), new DemoSimNet(), true);
  tabbedComponent->setCurrentTabIndex(0);

  setSize(1024, 768);
}

DemosMainComponent::~DemosMainComponent() {
  tabbedComponent = nullptr;
}

//==============================================================================
void DemosMainComponent::paint(Graphics& g) {
  g.fillAll(Colour(0xff323e44));
}

void DemosMainComponent::resized() {
  // auto b = getLocalBounds();
  // auto height = LookAndFeel::getDefaultLookAndFeel().getDefaultMenuBarHeight();
  // menuBar->setBounds(b.removeFromTop(height));

  tabbedComponent->setBounds(8, 8, getWidth() - 16, getHeight() - 16);
}
