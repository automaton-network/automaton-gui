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

struct ValidatorSlot {
  std::string difficulty;
  std::string owner;
  std::string last_claim_time;
};

class AutomatonContractData {
 public:
  std::string eth_url = "http://54.174.16.2:7545";
  std::string contract_address = "0xc6A2d391fe7471EEF1D17bba1035351439bEcbBE";
  std::string mask = "DEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEF";
  std::string minDifficulty = "DEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEF";
  uint32_t slots_number = 16;
  uint32_t slots_claimed = 0;
  std::vector<ValidatorSlot> slots;

  JUCE_DECLARE_SINGLETON(AutomatonContractData, true)

  CriticalSection criticalSection;
};
