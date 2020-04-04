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

#include <JuceHeader.h>
#include "Models/AbstractProxyModel.h"

class DemosMainComponent;
class AccountWindow;

class Account {
 public:
  Account() {
  }

  Account(const String& address) {
    m_address = address;
  }

  const String& getAddress() const noexcept {
    return m_address;
  }

  String getAlias() const noexcept {
    return m_config.getValue("account_alias");
  }

  PropertySet& getConfig() noexcept {
    return m_config;
  }

  bool operator==(const Account& other) const noexcept {
    return m_address == other.m_address;
  }

 private:
  String m_address;
  PropertySet m_config;
};

class AccountsModel : public AbstractListModel<Account>{
 public:
  int size() const override {
    return m_accounts.size();
  }

  Account getAt(int index) override {
    return m_accounts[index];
  }

  Account& getReferenceAt(int index) override {
    return m_accounts.getReference(index);
  }

  void addItem(Account account, bool sendNotification = true) {
    m_accounts.addIfNotAlreadyThere(account);

    if (sendNotification)
      notifyModelChanged();
  }

  void removeItem(const Account& account, bool sendNotification = true) {
    m_accounts.removeFirstMatchingValue(account);

    if (sendNotification)
      notifyModelChanged();
  }

 private:
  Array<Account> m_accounts;
};

class LoginComponent  : public Component
                      , public ComponentListener
                      , public Button::Listener
                      , public TableListBoxModel
                      , public AbstractListModelBase::Listener {
 public:
  LoginComponent(PropertiesFile* configFile);
  ~LoginComponent();

  void paint(Graphics&) override;
  void resized() override;
  void buttonClicked(Button* btn) override;
  void componentVisibilityChanged(Component& component) override;
  void modelChanged(AbstractListModelBase* base) override;
  void openAccount(Account* account);
  void removeAccount(const Account& account);

  // TableListBoxModel
  // ==============================================================================

  int getNumRows();
  void paintCell(Graphics& g,
                 int rowNumber, int columnId,
                 int width, int height,
                 bool rowIsSelected) override;
  void paintRowBackground(Graphics& g,
                          int rowNumber,
                          int width, int height,
                          bool rowIsSelected) override;
  void cellClicked(int rowNumber, int columnId, const MouseEvent&) override;
  void cellDoubleClicked(int rowNumber, int columnId, const MouseEvent&) override;

 private:
  AccountWindow* getWindowByAddress(const String& address);
  std::shared_ptr<AccountsModel> m_model;
  std::unique_ptr<TableListBox> m_accountsTable;
  std::unique_ptr<TextButton> m_importPrivateKeyBtn;
  OwnedArray<AccountWindow> m_accountWindows;
  PropertiesFile* m_configFile;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LoginComponent)
};
