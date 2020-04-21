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
#include "../Utils/AsyncTask.h"
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

static std::shared_ptr<eth_contract> getContract(status* resStatus) {
  const auto contract = AutomatonContractData::getInstance()->getContract();
  if (contract == nullptr)
    *resStatus = status::internal("Contract is NULL. Read appropriate contract data first.");

  return contract;
}

DEXManager::DEXManager(Config* config) {
  m_ethBalance = "Undefined";
  m_autoBalance = "Undefined";

  m_model = std::make_shared<OrdersModel>();

  m_privateKey = config->get_string("private_key");
  m_ethAddress = config->get_string("eth_address");
  m_ethAddressAlias = config->get_string("account_alias");
}

DEXManager::~DEXManager() {
}

std::shared_ptr<OrdersModel> DEXManager::getModel() {
  return m_model;
}

std::string DEXManager::getEthBalance() {
  return m_ethBalance;
}

std::string DEXManager::getAutoBalance() {
  return m_autoBalance;
}

static uint64 getNumOrders(std::shared_ptr<eth_contract> contract, status* resStatus) {
  *resStatus = contract->call("getOrdersLength", "");
  if (!resStatus->is_ok())
    return 0;

  const json jsonData = json::parse(resStatus->msg);
  const uint64 ordersLength = std::stoul((*jsonData.begin()).get<std::string>());
  return ordersLength;
}

static std::string ethBalance(const std::string& m_ethAddress
                              , status* resStatus) {
  *resStatus = eth_getBalance(AutomatonContractData::getInstance()->getUrl(), m_ethAddress);
  BigInteger balance;
  balance.parseString(resStatus->msg, 16);
  return balance.toString(10).toStdString();
}

static std::string autoBalance(std::shared_ptr<eth_contract> contract
    , const std::string& m_ethAddress
    , status* resStatus) {
  json jInput;
  jInput.push_back(m_ethAddress.substr(2));
  *resStatus = contract->call("balanceOf", jInput.dump());
  json j_output = json::parse(resStatus->msg);
  return  (*j_output.begin()).get<std::string>();
}

bool DEXManager::fetchOrders() {
  Array<Order::Ptr> orders;
  AsyncTask task([&](AsyncTask* task) {
    auto& s = task->m_status;

    auto contract = getContract(&s);
    if (!s.is_ok())
      return false;

    m_ethBalance = ethBalance(m_ethAddress, &s);
    if (!s.is_ok())
      return false;

    m_autoBalance = autoBalance(contract, m_ethAddress, &s);
    if (!s.is_ok())
      return false;

    m_model->clear(false);

    const auto numOfOrders = getNumOrders(contract, &s);
    if (!s.is_ok())
      return false;

    for (int i = 1; i <= numOfOrders; ++i) {
      json jInput;
      jInput.push_back(i);

      s = contract->call("getOrder", jInput.dump());

      if (!s.is_ok())
        return false;

      auto order = std::make_shared<Order>(String(s.msg));
      orders.add(order);
    }

    return true;
  }, "Fetching orders...");

  if (task.runThread()) {
    auto& s = task.m_status;
    if (s.is_ok()) {
      m_model->clear(false);
      m_model->addItems(orders, true);
      return true;
    } else {
      AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
                                       "ERROR",
                                       String("(") + String(s.code) + String(") :") + s.msg);
    }
  } else {
    AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
                                     "Fetching orders canceled!",
                                     "Current orders data may be broken.");
  }

  return false;
}
