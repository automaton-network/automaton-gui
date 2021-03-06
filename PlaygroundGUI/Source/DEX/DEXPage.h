/*
 * Automaton Playground
 * Copyright (c) 2020 The Automaton Authors.
 * Copyright (c) 2020 The automaton.network Authors.
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
#include <Login/Account.h>

#include "OrdersModel.h"

class OrdersUIModel;
class DEXManager;

class DEXPage : public Component
              , public Button::Listener
              , public AbstractListModelBase::Listener {
 public:
  DEXPage(Account::Ptr accountData);
  ~DEXPage();

  void paint(Graphics&) override;
  void resized() override;
  void buttonClicked(Button* buttonThatWasClicked) override;

  // AbstractListModelBase::Listener
  void modelChanged(AbstractListModelBase* model) override;

  void mouseDoubleClick(const MouseEvent& e) override;

 private:
  std::unique_ptr<Label> m_ethBalanceLabel;
  std::unique_ptr<Label> m_dexEthBalanceLabel;
  std::unique_ptr<Label> m_autoBalanceLabel;
  std::unique_ptr<OrdersUIModel> m_sellingUIModel;
  std::unique_ptr<OrdersUIModel> m_buyingUIModel;
  std::unique_ptr<Label> m_sellingLabel;
  std::unique_ptr<Label> m_buyingLabel;
  std::unique_ptr<TextButton> m_createSellOrderBtn;
  std::unique_ptr<TextButton> m_createBuyOrderBtn;
  std::unique_ptr<TableListBox> m_sellingTable;
  std::unique_ptr<TableListBox> m_buyingTable;
  std::shared_ptr<OrdersProxyModel> m_sellingProxyModel;
  std::shared_ptr<OrdersProxyModel> m_buyingProxyModel;
  Account::Ptr m_accountData;
  DEXManager* m_dexManager;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DEXPage)
};
