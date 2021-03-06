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
#include "../Utils/TasksManager.h"

#include <secp256k1_recovery.h>
#include <secp256k1.h>
#include <json.hpp>

#include "automaton/core/io/io.h"
#include "automaton/core/interop/ethereum/eth_contract_curl.h"
#include "automaton/core/interop/ethereum/eth_transaction.h"
#include "automaton/core/interop/ethereum/eth_helper_functions.h"
#include "automaton/core/crypto/cryptopp/Keccak_256_cryptopp.h"

using json = nlohmann::json;

using automaton::core::common::status;
using automaton::core::interop::ethereum::dec_to_i256;
using automaton::core::interop::ethereum::eth_contract;
using automaton::core::io::bin2hex;
using automaton::core::io::dec2hex;
using automaton::core::io::hex2bin;
using automaton::core::io::hex2dec;
using automaton::core::crypto::cryptopp::Keccak_256_cryptopp;

AutomatonContractData::AutomatonContractData(const Config& config) {
  loadAbi();
  m_config  = config;
  m_ethUrl = String(m_config.get_string("eth_url")).trim().toStdString();
  m_contractAddress = String(m_config.get_string("contract_address")).trim().toStdString();
  m_mask = String(m_config.get_string("mask")).trim().toStdString();
  m_minDifficulty = m_config.get_string("min_difficulty");
  m_slotsNumber = static_cast<uint32_t>(m_config.get_number("slots_number"));
  m_slotsClaimed = static_cast<uint32_t>(m_config.get_number("slots_claimed"));
}

AutomatonContractData::~AutomatonContractData() {
  stopOwnedTasks();
}

void AutomatonContractData::setData(const std::string& _eth_url,
                                    const std::string& _contractAddress,
                                    const std::string& _mask,
                                    const std::string& _min_difficulty,
                                    uint32_t _slots_number,
                                    uint32_t _slots_claimed,
                                    const ProposalThresholdData& proposalThresholdData,
                                    const std::vector<ValidatorSlot>& _slots) {
  ScopedLock sl(m_criticalSection);
  m_ethUrl = _eth_url;
  m_contractAddress = _contractAddress;
  m_mask = _mask;
  m_minDifficulty = _min_difficulty;
  m_slotsNumber = _slots_number;
  m_slotsClaimed = _slots_claimed;
  m_proposalThresholdData = proposalThresholdData;
  m_slots = _slots;

  m_config.set_string("eth_url", m_ethUrl);
  m_config.set_string("contract_address", m_contractAddress);
  m_config.set_string("mask", m_mask);
  m_config.set_string("min_difficulty", m_minDifficulty);
  m_config.set_number("slots_number", m_slotsNumber);
  m_config.set_number("slots_claimed", m_slotsClaimed);
  m_isLoaded = true;
  sendChangeMessage();
}

bool AutomatonContractData::readContract() {
  const std::string& url = m_ethUrl;
  const std::string& contractAddress = m_contractAddress;

  launchTask([&](AsyncTask* task){
    auto& s = task->m_status;
    eth_contract::register_contract(url, contractAddress, getAbi());
    auto contract = eth_contract::get_contract(contractAddress);
    if (contract == nullptr) {
      std::cout << "ERROR: Contract is NULL" << std::endl;
      s = status::internal("Contract is NULL!");
      return false;
    }

    task->setProgress(0.1);
    s = contract->call("numSlots", "");
    task->logStatus(s, "numSlots");
    if (!s.is_ok() || task->threadShouldExit())
      return false;

    if (s.msg.empty()) {
      s = status::internal("Invalid contract address!");
      std::cout << "ERROR: Invalid contract address" << std::endl;
      return false;
    }
    json j_output = json::parse(s.msg);
    auto slotsNumber = String(((*j_output.begin()).get<std::string>())).getIntValue();
    std::vector<ValidatorSlot> validatorSlots;
    validatorSlots.resize(slotsNumber);

    uint32_t step = 1024;
    for (uint32_t slot = 0; slot < slotsNumber; slot += step) {
      if (task->threadShouldExit()) {
        s = status::internal("Aborted");
        std::cout << "ERROR: Aborted!" << std::endl;
        return false;
      }
      if (step > slotsNumber - slot) {
        step = slotsNumber - slot;
      }
      task->setProgress((1.0 * slot) / slotsNumber);
      task->setStatusMessage(
          "Getting slot " + String(slot + step) + " of " + String(slotsNumber));

      json j_input;
      j_input.push_back(slot);
      j_input.push_back(step);
      std::string params = j_input.dump();

      // Fetch owners.
      s = contract->call("getOwners", params);
      if (!s.is_ok() || task->threadShouldExit()) {
        std::cout << "ERROR: " << s.msg << std::endl;
        task->logStatus(s, "getOwners");
        return false;
      }

      // Parse owners.
      j_output = json::parse(s.msg);
      std::vector<std::string> owners = (*j_output.begin()).get<std::vector<std::string> >();
      for (uint32_t i = 0; i < owners.size(); i++) {
        validatorSlots[slot + i].owner = owners[i];
      }

      // Fetch difficulties.
      s = contract->call("getDifficulties", params);
      if (!s.is_ok() || task->threadShouldExit()) {
        std::cout << "ERROR: " << s.msg << std::endl;
        task->logStatus(s, "getDifficulties");
        return false;
      }

      // Parse difficulties.
      j_output = json::parse(s.msg);
      std::vector<std::string> diff = (*j_output.begin()).get<std::vector<std::string> >();
      for (uint32_t i = 0; i < diff.size(); i++) {
        validatorSlots[slot + i].difficulty = dec_to_i256(false, diff[i]);
      }

      // Fetch last claim times.
      s = contract->call("getLastClaimTimes", params);
      if (!s.is_ok() || task->threadShouldExit()) {
        std::cout << "ERROR: " << s.msg << std::endl;
        task->logStatus(s, "getLastClaimTimes");
        return false;
      }

      // Parse last claim times.
      j_output = json::parse(s.msg);
      std::vector<std::string> last_claims = (*j_output.begin()).get<std::vector<std::string> >();
      for (uint32_t i = 0; i < last_claims.size(); i++) {
        validatorSlots[slot + i].last_claim_time = last_claims[i];
      }
    }
    task->setProgress(0);

    s = contract->call("mask", "");
    task->logStatus(s, "mask");

    j_output = json::parse(s.msg);
    auto mask = bin2hex(dec_to_i256(false, (*j_output.begin()).get<std::string>()));
    task->setStatusMessage("Mask: " + mask);

    s = contract->call("minDifficulty", "");
    task->logStatus(s, "minDifficulty");
    j_output = json::parse(s.msg);
    auto minDifficulty = bin2hex(dec_to_i256(false, (*j_output.begin()).get<std::string>()));
    task->setStatusMessage("MinDifficulty: " + minDifficulty);

    s = contract->call("proposalsData", "");
    if (!s.is_ok() || task->threadShouldExit()) {
      task->logStatus(s, "proposalsData");
      return false;
    }

    j_output = json::parse(s.msg);
    ProposalThresholdData proposalThresholdData = {String(j_output[0].get<std::string>()).getLargeIntValue(),
                                                   String(j_output[1].get<std::string>()).getLargeIntValue()};

    s = contract->call("numTakeOvers", "");
    task->logStatus(s, "numTakeOvers");
    j_output = json::parse(s.msg);
    std::string slots_claimed_string = (*j_output.begin()).get<std::string>();
    auto slotsClaimed = String(slots_claimed_string).getLargeIntValue();
    task->setStatusMessage("Number of slot claims: " + slots_claimed_string);
    task->setProgress(1.0);

    if (task->threadShouldExit())
      return false;

    s = status::ok();
    setData(url, contractAddress, mask, minDifficulty, slotsNumber, slotsClaimed,
            proposalThresholdData, validatorSlots);

    return true;
  }, nullptr, "Reading Contract...");

  return true;
}

std::shared_ptr<eth_contract> AutomatonContractData::getContract() {
  return eth_contract::get_contract(getAddress());
}

status AutomatonContractData::call(const std::string& f,
                                   const std::string& params,
                                   const std::string& privateKey,
                                   const std::string& value) {
  ScopedLock sl(m_criticalSection);
  if (auto contract = getContract())
    return getContract()->call(f, params, privateKey, value);

  return status::internal("Contract is NULL.");
}

std::string AutomatonContractData::getAbi() {
  ScopedLock sl(m_criticalSection);
  return m_contractAbi;
}

bool AutomatonContractData::loadAbi() {
  int file_size;
  const char* abi = BinaryData::getNamedResource("king_automaton_abi_json", file_size);
  if (abi == nullptr || file_size == 0) {
    return false;
  }

  m_contractAbi = std::string(abi, file_size);
  return true;
}

std::string AutomatonContractData::getUrl() const noexcept {
  ScopedLock sl(m_criticalSection);
  return m_ethUrl;
}

std::string AutomatonContractData::getAddress() const noexcept {
  ScopedLock sl(m_criticalSection);
  return m_contractAddress;
}

std::string AutomatonContractData::getMask() const noexcept {
  ScopedLock sl(m_criticalSection);
  return m_mask;
}

std::string AutomatonContractData::getMinDifficulty() const noexcept {
  ScopedLock sl(m_criticalSection);
  return m_minDifficulty;
}

uint32_t AutomatonContractData::getSlotsNumber() const noexcept {
  ScopedLock sl(m_criticalSection);
  return m_slotsNumber;
}

uint32_t AutomatonContractData::getSlotsClaimed() const noexcept {
  ScopedLock sl(m_criticalSection);
  return m_slotsClaimed;
}

ProposalThresholdData AutomatonContractData::getThresholdData() const noexcept {
  ScopedLock sl(m_criticalSection);
  return m_proposalThresholdData;
}

std::vector<ValidatorSlot> AutomatonContractData::getSlots() const {
  ScopedLock sl(m_criticalSection);
  return m_slots;
}

bool AutomatonContractData::isLoaded() const noexcept {
  return m_isLoaded;
}

Config& AutomatonContractData::getConfig() {
  return m_config;
}
