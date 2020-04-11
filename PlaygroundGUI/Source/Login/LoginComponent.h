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

#include "../Models/AbstractProxyModel.h"
#include "AccountsModel.h"

class DemosMainComponent;
class AccountWindow;
class ConfigFile;

class LoginComponent  : public Component
                      , public ComponentListener
                      , public Button::Listener
                      , public TableListBoxModel
                      , public AbstractListModelBase::Listener {
 public:
  LoginComponent(ConfigFile* configFile);
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

  int getNumRows() override;
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
  void switchLoginState(bool isNetworkConfig);

 private:
  AccountWindow* getWindowByAddress(const String& address);
  std::shared_ptr<AccountsModel> m_model;
  std::unique_ptr<Drawable> m_logo;
  std::unique_ptr<TableListBox> m_accountsTable;
  std::unique_ptr<TextButton> m_importPrivateKeyBtn;
  std::unique_ptr<Label> m_rpcLabel;
  std::unique_ptr<TextEditor> m_rpcEditor;
  std::unique_ptr<Label> m_contractAddrLabel;
  std::unique_ptr<TextEditor> m_contractAddrEditor;
  std::unique_ptr<TextButton> m_readContractBtn;

  OwnedArray<AccountWindow> m_accountWindows;
  ConfigFile* m_configFile;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LoginComponent)
};
