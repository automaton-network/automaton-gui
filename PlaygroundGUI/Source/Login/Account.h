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

#include "JuceHeader.h"
#include "../Config/Config.h"

class ProposalsManager;
class DEXManager;
class AutomatonContractData;
class AccountConfig;

class Account : public std::enable_shared_from_this<Account> {
 public:
  Account(AccountConfig* config, std::shared_ptr<AutomatonContractData> contractData);
  ~Account();

  void initManagers();
  void clearManagers();

  std::shared_ptr<AutomatonContractData> getContractData() const noexcept;
  ProposalsManager* getProposalsManager();
  DEXManager* getDexManager();

  const std::string& getAddress() const noexcept;
  std::string getPrivateKey() const noexcept;
  std::string getAlias() const noexcept;
  std::string getEthBalance() const noexcept;
  std::string getAutoBalance() const noexcept;

  bool operator==(const Account& other) const noexcept;

  using Ptr = std::shared_ptr<Account>;

 private:
  void setBalance(const std::string& ethBalance, const std::string& autoBalance);

  std::string m_ethBalance;
  std::string m_autoBalance;
  std::string m_address;
  AccountConfig* m_config;
  std::shared_ptr<AutomatonContractData> m_contractData;
  std::unique_ptr<ProposalsManager> m_proposalsManager;
  std::unique_ptr<DEXManager> m_dexManager;

  friend class DEXManager;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Account)
};
