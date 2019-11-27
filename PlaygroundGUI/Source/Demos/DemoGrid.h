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

#pragma once

#include "../../JuceLibraryCode/JuceHeader.h"
#include "../Components/ValidatorGrid.h"

static const char* TEST_OWNER = "thisistestownerthisistestowner00";

class SlotsModelTest: public SlotsModelInterface {
 public:
  SlotsModelTest();

  uint32_t get_slots_number();

  std::string get_slot_owner(uint32_t slot_index);

  std::string get_slot_difficulty(uint32_t slot_index);

 private:
  uint32_t slots_number;
  std::vector<std::string> owners;
  std::vector<std::string> difficulties;
};

class DemoGrid: public Component, private Timer {
 public:
  DemoGrid(std::shared_ptr<SlotsModelInterface> model, const std::string& owner);
  ~DemoGrid();

  void resized() override;
  void update();

 private:
  ValidatorGrid grid_component;
  void timerCallback() override;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DemoGrid)
};
