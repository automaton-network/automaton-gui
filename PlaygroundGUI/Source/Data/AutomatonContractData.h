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
#include "../Login/AccountsModel.h"
#include "automaton/core/interop/ethereum/eth_contract_curl.h"
#include "automaton/core/common/status.h"

struct ValidatorSlot {
  std::string difficulty;
  std::string owner;
  std::string last_claim_time;
};

class AutomatonContractData : public ChangeBroadcaster {
 public:
  using Ptr = std::shared_ptr<AutomatonContractData>;

  AutomatonContractData(const Config& config);
  ~AutomatonContractData();
  void setData(const std::string& _eth_url,
               const std::string& _contractAddress,
               const std::string& _mask,
               const std::string& _min_difficulty,
               uint32_t _slots_number,
               uint32_t _slots_claimed,
               const std::vector<ValidatorSlot>& _slots);

  bool readContract();
  std::shared_ptr<automaton::core::interop::ethereum::eth_contract> getContract();
  automaton::core::common::status call(const std::string& f,
                                       const std::string& params,
                                       const std::string& privateKey = "",
                                       const std::string& value = "");

  bool loadAbi();
  std::string getAbi();
  std::string getUrl() const noexcept;
  std::string getAddress() const noexcept;
  std::string getMask() const noexcept;
  std::string getMinDifficulty() const noexcept;
  uint32_t getSlotsNumber() const noexcept;
  uint32_t getSlotsClaimed() const noexcept;
  std::vector<ValidatorSlot> getSlots() const;
  bool isLoaded() const noexcept;

  std::string m_contractAbi;
  std::string m_ethUrl;
  std::string m_contractAddress;
  std::string m_mask;
  std::string m_minDifficulty;
  uint32_t m_slotsNumber;
  uint32_t m_slotsClaimed;
  std::vector<ValidatorSlot> m_slots;

  CriticalSection m_criticalSection;

  Config& getConfig();

 private:
  bool m_isLoaded = false;
  Config m_config;
};
