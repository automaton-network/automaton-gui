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

class AutomatonContractData : DeletedAtShutdown {
 public:
  AutomatonContractData();
  ~AutomatonContractData();
  void init(PropertySet* _config);
  void setData(const std::string& _eth_url,
               const std::string& _contractAddress,
               const std::string& _mask,
               const std::string& _min_difficulty,
               uint32_t _slots_number,
               uint32_t _slots_claimed,
               const std::vector<ValidatorSlot>& _slots);

  const std::string& get_abi();
  bool load_abi();

  std::string contract_abi;
  std::string eth_url;
  std::string contract_address;
  std::string mask;
  std::string min_difficulty;
  uint32_t slots_number;
  uint32_t slots_claimed;
  std::vector<ValidatorSlot> slots;

  JUCE_DECLARE_SINGLETON(AutomatonContractData, true)

  CriticalSection criticalSection;

 private:
  PropertySet* config;
};
