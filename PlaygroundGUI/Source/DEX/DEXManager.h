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
#include <Utils/TasksOwner.h>
#include "Order.h"
#include "Login/Account.h"
#include "Config/Config.h"

class OrdersModel;

class DEXManager : public TasksOwner {
 public:
  DEXManager(Account::Ptr accountData);
  ~DEXManager();

  std::shared_ptr<OrdersModel> getModel();

  bool fetchOrders();
  bool createSellOrder(const String& amountAUTO, const String& amountETHwei);
  bool createBuyOrder(const String& amountAUTO, const String& amountETHwei);
  bool cancelOrder(Order::Ptr order);
  bool acquireBuyOrder(Order::Ptr order);
  bool acquireSellOrder(Order::Ptr order);
  bool getDexEthBalance(const String& address);
  bool withdrawEthFromDEX(const String& amountETHwei);


 private:
  std::shared_ptr<OrdersModel> m_model;

  Account::Ptr m_accountData;
  std::shared_ptr<AutomatonContractData> m_contractData;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DEXManager)
};
