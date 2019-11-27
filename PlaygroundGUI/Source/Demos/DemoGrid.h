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

static const char* TEST_OWNER = "thisistestownerthisistestowner00";

class SlotsModelInterface {
 public:
  virtual uint32_t get_slots_number() = 0;
  virtual std::string get_slot_owner(uint32_t slot_index) = 0;
  virtual std::string get_slot_difficulty(uint32_t slot_index) = 0;
};

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
  DemoGrid(std::shared_ptr<SlotsModelInterface> model, const std::string& owner,
      uint64_t initial_component_size, bool allow_text_over_slots);
  ~DemoGrid();

  void paint(Graphics& g) override;
  void resized() override;
  void update();

  void set_owner(const std::string& new_owner);
  void allow_text_over_slots(bool allow);

 private:
  struct slot {
    std::string difficulty = "";
    std::string owner = "";
    bool is_mine = false;
    uint32_t bits = 0;
  };

  std::shared_ptr<SlotsModelInterface> model;
  uint32_t slots_number;
  std::vector<slot> slots;
  uint32_t initial_leading_bits = 4;
  uint32_t min_leading_bits = 257;
  uint32_t max_leading_bits = 0;
  std::string owner;

  uint32_t component_start_x = 50;
  uint32_t component_start_y = 50;
  uint32_t component_side_size_px = 600;  // will be adjusted
  uint32_t slots_per_side;  // = sqrt(slots_number).ceil()
  uint32_t slot_size;  // ~= component_size_size_px / squares_per_side
  uint32_t gap;  // no gap if slots are too many
  bool allowed_text_over_slots = true;

  void timerCallback() override;

  uint32_t leading_bits(const std::string& s);

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DemoGrid)
};
