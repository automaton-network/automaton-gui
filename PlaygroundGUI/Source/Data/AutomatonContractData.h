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
    eth_url = conf->get_string("eth_url");  // = "http://54.174.16.2:7545";
    contract_address = conf->get_string("contract_address");
    // "0xc6A2d391fe7471EEF1D17bba1035351439bEcbBE";
    mask = conf->get_string("mask");
    // "DEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEF";
    minDifficulty = conf->get_string("min_difficulty");
    // "DEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEF";
    auto n = conf->get_number("slots_number");
    slots_number = n != 0 ? n : 16;
    n = conf->get_number("slots_claimed");
    slots_claimed = n;

    auto contract_abi = conf->get_json("contract_abi");

    // TEST
    // conf->set_and_save("eth_url", '"' + eth_url + '"');
    // conf->set_and_save("contract_address", '"' + contract_address + '"');
    // conf->set_and_save("mask", '"' + mask + '"');
    // conf->set_and_save("min_difficulty", '"' + minDifficulty + '"');
    // conf->set_and_save("slots_number", std::to_string(slots_number));
    // conf->set_and_save("slots_claimed", std::to_string(slots_claimed));
    // conf->set_and_save("contract_abi", contract_abi);
  }

  std::string eth_url;
  std::string contract_address;
  std::string mask;
  std::string minDifficulty;
  uint32_t slots_number;
  uint32_t slots_claimed;
  std::vector<ValidatorSlot> slots;

  JUCE_DECLARE_SINGLETON(AutomatonContractData, true)

  CriticalSection criticalSection;
};
