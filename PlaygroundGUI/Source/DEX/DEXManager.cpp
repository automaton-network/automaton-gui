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

#include "DEXManager.h"
#include "OrdersModel.h"
#include "../Utils/TasksManager.h"
#include "../Data/AutomatonContractData.h"
#include "automaton/core/interop/ethereum/eth_contract_curl.h"
#include "automaton/core/interop/ethereum/eth_helper_functions.h"
#include "automaton/core/common/status.h"

using automaton::core::common::status;
using automaton::core::interop::ethereum::eth_contract;
using automaton::core::interop::ethereum::eth_getBalance;
using automaton::core::io::bin2hex;
using automaton::core::io::dec2hex;
using automaton::core::io::hex2dec;

DEXManager::DEXManager(Account::Ptr accountData)
: m_model(std::make_shared<OrdersModel>()),
  m_accountData(accountData) {
  m_contractData = m_accountData->getContractData();
}

DEXManager::~DEXManager() {
}

std::shared_ptr<OrdersModel> DEXManager::getModel() {
  return m_model;
}

static uint64 getNumOrders(AutomatonContractData::Ptr contract, status* resStatus) {
  *resStatus = contract->call("getOrdersLength", "");
  if (!resStatus->is_ok())
    return 0;

  const json jsonData = json::parse(resStatus->msg);
  const uint64 ordersLength = std::stoul((*jsonData.begin()).get<std::string>());
  return ordersLength;
}

static std::string getEthBalance(AutomatonContractData::Ptr contract
    , const std::string& m_ethAddress
    , status* resStatus) {
  *resStatus = eth_getBalance(contract->getUrl(), m_ethAddress);
  BigInteger balance;
  balance.parseString(resStatus->msg, 16);
  return balance.toString(10).toStdString();
}

static std::string getAutoBalance(AutomatonContractData::Ptr contract
    , const std::string& m_ethAddress
    , status* resStatus) {
  json jInput;
  jInput.push_back(m_ethAddress.substr(2));
  *resStatus = contract->call("balanceOf", jInput.dump());
  json j_output = json::parse(resStatus->msg);
  return  (*j_output.begin()).get<std::string>();
}

bool DEXManager::fetchOrders() {
  TasksManager::launchTask([&](AsyncTask* task) {
    auto& s = task->m_status;

    auto ethBalance = getEthBalance(m_contractData, m_accountData->getAddress(), &s);
    if (!s.is_ok())
      return false;

    auto autoBalance = getAutoBalance(m_contractData, m_accountData->getAddress(), &s);
    if (!s.is_ok())
      return false;

    m_accountData->setBalance(ethBalance, autoBalance);

    m_model->clear(NotificationType::dontSendNotification);

    const auto numOfOrders = getNumOrders(m_contractData, &s);
    if (!s.is_ok())
      return false;

    Array<Order::Ptr> orders;
    for (int i = 1; i <= numOfOrders; ++i) {
      json jInput;
      jInput.push_back(i);

      s = m_contractData->call("getOrder", jInput.dump());

      if (!s.is_ok())
        return false;

      auto order = std::make_shared<Order>(String(s.msg));
      orders.add(order);
    }
    m_model->clear(NotificationType::dontSendNotification);
    m_model->addItems(orders, NotificationType::sendNotificationAsync);

    return true;
  }, [=](AsyncTask* task) {
  }, "Fetching orders...");

  return true;
}
