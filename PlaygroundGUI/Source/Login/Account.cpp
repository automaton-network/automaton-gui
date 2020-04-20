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

#include "Account.h"
#include "Data/AutomatonContractData.h"
#include "../Data/AutomatonContractData.h"
#include "../Proposals/ProposalsManager.h"
#include "../DEX/DEXManager.h"

Account::Account(const Config& config,
                 const std::string& address,
                 std::shared_ptr<AutomatonContractData> contractData) : m_contractData(contractData) {
  m_config = config;
  m_address = address;
  m_ethBalance = "Undefined";
  m_autoBalance = "Undefined";
}

void Account::initManagers() {
  m_proposalsManager = std::make_unique<ProposalsManager>(shared_from_this());
  m_dexManager = std::make_unique<DEXManager>(shared_from_this());
}

Account::~Account() {
}

std::shared_ptr<AutomatonContractData> Account::getContractData() const noexcept {
  return m_contractData;
}

ProposalsManager* Account::getProposalsManager() {
  return m_proposalsManager.get();
}

DEXManager* Account::getDexManager() {
  return m_dexManager.get();
}

const std::string& Account::getAddress() const noexcept {
  return m_address;
}

std::string Account::getPrivateKey() const noexcept {
  return m_config.get_string("private_key");
}

std::string Account::getAlias() const noexcept {
  return m_config.get_string("account_alias");
}

Config& Account::getConfig() noexcept {
  return m_config;
}

bool Account::operator==(const Account& other) const noexcept {
  return m_address == other.m_address && m_contractData == other.m_contractData;
}

std::string Account::getAutoBalance() const noexcept {
  return m_ethBalance;
}

std::string Account::getEthBalance() const noexcept {
  return m_autoBalance;
}

void Account::setBalance(const std::string& ethBalance, const std::string& autoBalance) {
  m_ethBalance = ethBalance;
  m_autoBalance = autoBalance;
}
