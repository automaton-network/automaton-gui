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

#include "DemoGrid.h"

DemoGrid::DemoGrid(std::shared_ptr<SlotsModelInterface> model, const std::string& owner):
    grid_component(model, owner, true) {
  addAndMakeVisible(grid_component);
  grid_component.setBounds(50, 50, 600, 600);
  startTimer(1000);
}

DemoGrid::~DemoGrid() {}

void DemoGrid::update() {
  grid_component.update();
}

void DemoGrid::resized() {}

void DemoGrid::timerCallback() {
  update();
  repaint();
}

// ======================= TEST ===============================

SlotsModelTest::SlotsModelTest() {
  srand(time(NULL));
  slots_number = 1000;
  owners.resize(slots_number, std::string('0', 32));
  difficulties.resize(slots_number, std::string('0', 32));
}

uint32_t SlotsModelTest::get_slots_number() {
  return slots_number;
}

std::string SlotsModelTest::get_slot_owner(uint32_t slot_index) {
  return owners[slot_index];
}

std::string SlotsModelTest::get_slot_difficulty(uint32_t slot_index) {
  uint32_t k = std::rand() % 100;
  if (k > 65) {
    for (uint i = 0; i < 32; ++i) {
      owners[slot_index][i] = 'a' + std::rand() % 26;
      difficulties[slot_index][i] = std::rand() % 256;
    }
    if (k > 97) {
      owners[slot_index] = TEST_OWNER;
    }
  }
  return difficulties[slot_index];
}
