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
#include "Order.h"
#include "../Config/Config.h"

class OrdersModel;

class DEXManager {
 public:
  DEXManager(Config* config);
  ~DEXManager();
  std::shared_ptr<OrdersModel> getModel();
  std::string getEthBalance();
  std::string getAutoBalance();

  bool fetchOrders();

 private:
  std::shared_ptr<OrdersModel> m_model;
  std::string m_ethBalance;
  std::string m_autoBalance;

  std::string m_privateKey;
  std::string m_ethAddress;
  std::string m_ethAddressAlias;
};
