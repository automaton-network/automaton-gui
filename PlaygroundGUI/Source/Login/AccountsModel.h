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

#ifndef ACCOUNTSMODEL_H_INCLUDED
#define ACCOUNTSMODEL_H_INCLUDED

#include "../Models/AbstractProxyModel.h"
#include "../Config/Config.h"

class Account {
 public:
  Account();
  Account(const String& address);

  const String& getAddress() const noexcept;
  String getAlias() const noexcept;
  Config& getConfig() noexcept;

  bool operator==(const Account& other) const noexcept;

 private:
  String m_address;
  Config m_config;
};

class AccountsModel : public AbstractListModel<Account> {
 public:
  int size() const override;
  Account getAt(int index) override;
  Account& getReferenceAt(int index) override;

  void addItem(const Account& account, bool sendNotification = true);
  void removeItem(const Account& account, bool sendNotification = true);

 private:
  Array<Account> m_accounts;
};

#endif  // ACCOUNTSMODEL_H_INCLUDED
