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

#include  "AutomatonContractData.h"

JUCE_IMPLEMENT_SINGLETON(AutomatonContractData)

AutomatonContractData::AutomatonContractData() {
}

AutomatonContractData::~AutomatonContractData() {
  clearSingletonInstance();
}

void AutomatonContractData::init(Config* _config) {
  config  = _config;
  load_abi();

  eth_url = config->get_string("eth_url");
  contract_address = config->get_string("contract_address");
  mask = config->get_string("mask");
  min_difficulty = config->get_string("min_difficulty");
  slots_number = static_cast<uint32_t>(config->get_number("slots_number"));
  slots_claimed = static_cast<uint32_t>(config->get_number("slots_claimed"));
}

void AutomatonContractData::setData(const std::string& _eth_url,
                                    const std::string& _contractAddress,
                                    const std::string& _mask,
                                    const std::string& _min_difficulty,
                                    uint32_t _slots_number,
                                    uint32_t _slots_claimed,
                                    const std::vector<ValidatorSlot>& _slots) {
  ScopedLock sl(criticalSection);
  eth_url = _eth_url;
  contract_address = _contractAddress;
  mask = _mask;
  min_difficulty = _min_difficulty;
  slots_number = _slots_number;
  slots_claimed = _slots_claimed;
  slots = _slots;

  config->set_string("eth_url", eth_url);
  config->set_string("contract_address", contract_address);
  config->set_string("mask", mask);
  config->set_string("min_difficulty", min_difficulty);
  config->set_number("slots_number", slots_number);
  config->set_number("slots_claimed", slots_claimed);
}

const std::string &AutomatonContractData::get_abi() {
  return contract_abi;
}

bool AutomatonContractData::load_abi() {
  int file_size;
  const char* abi = BinaryData::getNamedResource("king_automaton_abi_json", file_size);
  if (abi == nullptr || file_size == 0) {
    return false;
  }

  contract_abi = std::string(abi, file_size);
  return true;
}
