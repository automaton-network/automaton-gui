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

int AccountsModel::size() const {
  return m_accounts.size();
}

Account::Ptr AccountsModel::getAt(int index) {
  return m_accounts[index];
}

Account::Ptr& AccountsModel::getReferenceAt(int index) {
  return m_accounts.getReference(index);
}

void AccountsModel::addItem(Account::Ptr account, NotificationType notification) {
  m_accounts.addIfNotAlreadyThere(account);
  notifyModelChanged(notification);
}

void AccountsModel::removeItem(Account::Ptr account, NotificationType notification) {
  m_accounts.removeFirstMatchingValue(account);
  notifyModelChanged(notification);
}
