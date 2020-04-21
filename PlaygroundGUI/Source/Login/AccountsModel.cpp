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


#include "AccountsModel.h"

AccountConfig::AccountConfig() {
}

AccountConfig::AccountConfig(const String& address) {
  m_address = address;
}

const String& AccountConfig::getAddress() const noexcept {
  return m_address;
}

String AccountConfig::getPrivateKey() const noexcept {
  return m_config.get_string("private_key");
}

String AccountConfig::getAlias() const noexcept {
  return m_config.get_string("account_alias");
}

Config& AccountConfig::getConfig() noexcept {
  return m_config;
}

bool AccountConfig::operator==(const AccountConfig& other) const noexcept {
  return m_address == other.m_address;
}

int AccountsModel::size() const {
  return m_accounts.size();
}

AccountConfig AccountsModel::getAt(int index) {
  return m_accounts[index];
}

AccountConfig& AccountsModel::getReferenceAt(int index) {
  return m_accounts.getReference(index);
}

void AccountsModel::addItem(const AccountConfig& account, NotificationType notification) {
  m_accounts.addIfNotAlreadyThere(account);
  notifyModelChanged(notification);
}

void AccountsModel::removeItem(const AccountConfig& account, NotificationType notification) {
  m_accounts.removeFirstMatchingValue(account);
  notifyModelChanged(notification);
}
