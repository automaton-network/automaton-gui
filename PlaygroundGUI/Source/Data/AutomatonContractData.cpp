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

void AutomatonContractData::init(PropertySet* _config) {
  config  = _config;
  load_abi();

  eth_url = config->getValue("eth_url", "http://127.0.0.1:7545").toStdString();
  contract_address = config->getValue("contract_address", "0xc6A2d391fe7471EEF1D17bba1035351439bEcbBE").toStdString();
  mask = config->getValue("mask", "DEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEF").toStdString();
  min_difficulty = config->getValue("min_difficulty",
                                    "DEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEFDEADBEEF").toStdString();
  slots_number = static_cast<uint32_t>(config->getIntValue("slots_number", 16));
  slots_claimed = static_cast<uint32_t>(config->getIntValue("slots_claimed", 0));
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

  config->setValue("eth_url", String(eth_url));
  config->setValue("contract_address", String(contract_address));
  config->setValue("mask", String(mask));
  config->setValue("min_difficulty", String(min_difficulty));
  config->setValue("slots_number", static_cast<int>(slots_number));
  config->setValue("slots_claimed", static_cast<int>(slots_claimed));
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
