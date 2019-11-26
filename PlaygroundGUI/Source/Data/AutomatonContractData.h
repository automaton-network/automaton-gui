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
#include "../Config/Config.h"

struct ValidatorSlot {
  std::string difficulty;
  std::string owner;
  std::string last_claim_time;
};

class AutomatonContractData {
 public:
  AutomatonContractData() {
    auto conf = Config::getInstance();
    eth_url = conf->get_string("eth_url");
    contract_address = conf->get_string("contract_address");
    mask = conf->get_string("mask");
    min_difficulty = conf->get_string("min_difficulty");
    try {
      slots_number = conf->get_number("slots_number");
    } catch (...) {
      slots_number = 0;
    }
    try {
      slots_claimed = conf->get_number("slots_claimed");
    } catch (...) {
      slots_claimed = 0;
    }

    conf->get_abi();

    // TEST
    conf->lock();
    conf->set_number("test_field", 17);
    conf->set_json("test_field2", "[\"non\",\"empty\"]");
    conf->save_to_local_file();
    conf->unlock();
  }

  std::string eth_url;
  std::string contract_address;
  std::string mask;
  std::string min_difficulty;
  uint32_t slots_number;
  uint32_t slots_claimed;
  std::vector<ValidatorSlot> slots;

  JUCE_DECLARE_SINGLETON(AutomatonContractData, true)

  CriticalSection criticalSection;
};
