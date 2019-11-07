/*
 * Automaton Playground
 * Copyright (c) 2017-2018 The Automaton Authors.
 * Copyright (c) 2017-2018 The automaton.network Authors.
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

#pragma once

#include "../../JuceLibraryCode/JuceHeader.h"

#include "automaton/core/crypto/cryptopp/secure_random_cryptopp.h"
#include "automaton/core/crypto/cryptopp/SHA256_cryptopp.h"
#include "automaton/core/io/io.h"

class DemoMiner:
  public Component,
  public Button::Listener,
  private Timer {
 public:
  //==============================================================================
  DemoMiner();
  ~DemoMiner();

  void paint(Graphics& g) override;
  void resized() override;

  void update();

  // Button::Listener overrides.
  void buttonClicked(Button* btn) override;

 private:
  OwnedArray<Component> components;

  void addComponent(Component* c) {
    components.add(c);
    addAndMakeVisible(c);
  }

  TextButton* TB(String text, int x, int y, int w, int h) {
    TextButton* tb = new TextButton(text);
    tb->addListener(this);
    addComponent(tb);
    tb->setBounds(x, y, w, h);

    tb->setColour(TextButton::textColourOffId,  Colours::black);
    tb->setColour(TextButton::textColourOnId,   Colours::black);
    tb->setColour(TextButton::buttonColourId,   Colours::white);
    tb->setColour(TextButton::buttonOnColourId, Colours::cyan.brighter());

    return tb;
  }

  void GTB(int gid, int def, StringArray texts, int x, int y, int w, int h) {
    for (unsigned int i = 0; i < texts.size(); i++) {
      String text = texts[i];
      TextButton* tb = TB(text, x, y, w, h);
      tb->setClickingTogglesState(true);
      tb->setRadioGroupId(gid);
      if (i == def) {
        tb->setToggleState(true, dontSendNotification);
      }

      tb->setConnectedEdges(
          ((i == 0) ? 0 : Button::ConnectedOnLeft) |
          ((i == (texts.size() - 1)) ? 0 : Button::ConnectedOnRight));

      x += w;
    }
  }

  Label* LBL(String text, int x, int y, int w, int h) {
    Label* lbl = new Label("", text);
    addComponent(lbl);
    lbl->setBounds(x, y, w, h);
    lbl->setColour(Label::textColourId, Colours::white);
    lbl->setJustificationType(Justification::centred);
    return lbl;
  }

  struct slot {
    Colour bg;
    Colour fg;
    std::string diff;
    int owner;
    int tm;
    uint32_t bits;
    slot()
      : diff(32, 0)
      , bits(0) {}
  };

  automaton::core::crypto::cryptopp::secure_random_cryptopp secure_rand;

  const uint64 supply_cap = 1UL << 40;
  uint32 m = 32;
  uint32 n = 32;
  uint32 sz = 8;
  uint32 gap = 1;

  uint64 reward_per_period = 1280000;
  double periods_per_day = 1.0;
  uint32 mask1 = 0;
  uint32 mask2 = 0;
  uint32 mask3 = 0;

  slot slots[256][256];
  uint64 total_balance = 0;
  uint64 total_supply = 0;

  unsigned int max_leading_bits = 4;
  unsigned int min_leading_bits = 0;
  unsigned int initial_difficulty_bits = 4;
  uint64 tx_count = 0;

  unsigned int t;

  uint32 mining_power = 0;

  void timerCallback() override;

  static int mp[];

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DemoMiner)
};
