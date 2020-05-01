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

#include <json.hpp>

#include "DEXManager.h"
#include "OrdersModel.h"
#include "Utils/TasksManager.h"
#include "Utils/Utils.h"
#include "Data/AutomatonContractData.h"

#include "automaton/core/interop/ethereum/eth_contract_curl.h"
#include "automaton/core/interop/ethereum/eth_helper_functions.h"
#include "automaton/core/interop/ethereum/eth_transaction.h"
#include "automaton/core/common/status.h"

using automaton::core::common::status;
using automaton::core::interop::ethereum::eth_contract;
using automaton::core::interop::ethereum::eth_getBalance;
using automaton::core::interop::ethereum::eth_transaction;
using automaton::core::interop::ethereum::eth_getTransactionCount;
using automaton::core::interop::ethereum::encode;
using automaton::core::io::bin2hex;

DEXManager::DEXManager(Account::Ptr accountData)
    : m_model(std::make_shared<OrdersModel>())
    , m_accountData(accountData) {
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

static std::string getEthBalance(AutomatonContractData::Ptr contract,
                                 const std::string& m_ethAddress,
                                 status* resStatus) {
  *resStatus = eth_getBalance(contract->getUrl(), m_ethAddress);
  BigInteger balance;
  balance.parseString(resStatus->msg, 16);
  return balance.toString(10).toStdString();
}

static std::string getAutoBalance(AutomatonContractData::Ptr contract,
                                  const std::string& m_ethAddress,
                                  status* resStatus) {
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
    for (size_t i = 1; i <= numOfOrders; ++i) {
      json jInput;
      jInput.push_back(i);

      s = m_contractData->call("getOrder", jInput.dump());

      if (!s.is_ok())
        return false;

      const auto order = std::make_shared<Order>(i, String(s.msg));
      // Don't add removed orders
      if (order->getType() != Order::Type::None)
        orders.add(order);
    }
    m_model->clear(NotificationType::dontSendNotification);
    m_model->addItems(orders, NotificationType::sendNotificationAsync);

    return true;
  }, [=](AsyncTask* task) {
  }, "Fetching orders...", m_accountData);

  return true;
}

bool DEXManager::createSellOrder(const String& amountAUTO, const String& amountETH) {
  const auto orderName = "(" + Utils::fromWei(CoinUnit::AUTO, amountAUTO) + " AUTO -> "
                          + Utils::fromWei(CoinUnit::ether, amountETH) + " ETH)";
  const auto topicName = "Create sell order + " + orderName;
  TasksManager::launchTask([=](AsyncTask* task) {
    auto& s = task->m_status;

    task->setProgress(0.1);

    json jSellOrder;
    jSellOrder.push_back(amountAUTO.toStdString());
    jSellOrder.push_back(amountETH.toStdString());

    task->setProgress(0.5);

    s = m_contractData->call("sell", jSellOrder.dump(), m_accountData->getPrivateKey());
    if (!s.is_ok())
      return false;

    DBG("Call result: " << s.msg << "\n");
    task->setProgress(1.0);

    task->setStatusMessage("Sell Order " + orderName + " successfully created");

    return true;
  }, [=](AsyncTask* task) {
  }, topicName, m_accountData);

  return true;
}

bool DEXManager::createBuyOrder(const String& amountAUTO, const String& amountETH) {
  const auto orderName = "(" + Utils::fromWei(CoinUnit::ether, amountETH) + " ETH -> "
                          + Utils::fromWei(CoinUnit::AUTO, amountAUTO) + " AUTO)";
  const auto topicName = "Create buy order + " + orderName;
  TasksManager::launchTask([=](AsyncTask* task) {
    auto& s = task->m_status;

    task->setProgress(0.1);

    json jBuyOrder;
    jBuyOrder.push_back(amountAUTO.toStdString());

    json jSignature;
    jSignature.push_back("uint256");

    std::stringstream txData;
    txData << "d96a094a" << bin2hex(encode(jSignature.dump(), jBuyOrder.dump()));

    s = eth_getTransactionCount(m_contractData->getUrl(), m_accountData->getAddress());
    auto nonce = s.is_ok() ? s.msg : "0";
    if (!s.is_ok())
      return false;

    if (nonce.substr(0, 2) == "0x") {
      nonce = nonce.substr(2);
    }
    task->setProgress(0.5);

    BigInteger intAmountETH;
    intAmountETH.parseString(amountETH, 10);
    const String hexAmountETH = intAmountETH.toString(16);

    eth_transaction transaction;
    transaction.nonce = nonce;
    transaction.gas_price = "1388";  // 5 000
    transaction.gas_limit = "5B8D80";  // 6M
    transaction.to = m_contractData->getAddress().substr(2);
    transaction.value = hexAmountETH.toStdString();
    transaction.data = txData.str();
    transaction.chain_id = "01";
    s = m_contractData->call("buy", transaction.sign_tx(m_accountData->getPrivateKey()));

    if (!s.is_ok())
      return false;

    DBG("Call result: " << s.msg << "\n");
    task->setProgress(1.0);

    task->setStatusMessage("Buy Order " + orderName + " successfully created");

    return true;
  }, [=](AsyncTask* task) {
  }, topicName, m_accountData);

  return true;
}
