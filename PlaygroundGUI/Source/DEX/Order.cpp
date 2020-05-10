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

#include "Order.h"

#include <json.hpp>
#include <string>

#include "Utils/Utils.h"

using json = nlohmann::json;


Order::Order(uint64 id, const String& jsonString)
    : m_id(id) {
  json jsonData = json::parse(jsonString.toStdString());

  const auto amountAUTOstr = jsonData.at(0).get<std::string>();
  const auto amountETHstr = jsonData.at(1).get<std::string>();

  m_auto = Utils::fromWei(CoinUnit::AUTO, amountAUTOstr);
  m_eth = Utils::fromWei(CoinUnit::ether, amountETHstr);
  m_price = Utils::divideBigInt(amountETHstr, amountAUTOstr, 10);

  m_owner = jsonData.at(2).get<std::string>();
  m_type = static_cast<Order::Type>(std::stoul(jsonData.at(3).get<std::string>()));
}

String Order::getOrderDescription(Type orderType,
                                  const String& amountAUTO, const String& amountETH,
                                  bool convertFromWei) {
  String amountAUTOdescription;
  String amountETHdescription;

  if (convertFromWei) {
    amountAUTOdescription = Utils::fromWei(CoinUnit::AUTO, amountAUTO);
    amountETHdescription = Utils::fromWei(CoinUnit::ether, amountETH);
  } else {
    amountAUTOdescription = amountAUTO;
    amountETHdescription = amountETH;
  }

  switch (orderType) {
    case Type::Buy:
      return "(" + amountETHdescription + " ETH -> " + amountAUTOdescription + " AUTO)";

    case Type::Sell:
      return "(" + amountAUTOdescription + " AUTO -> " + amountETHdescription + " ETH)";

    // TODO(Kirill) add support for Auction type in future
    case Type::Auction:
    default:
      return String("Invalid order");
  }
}

String Order::getDescription() const noexcept {
  return getOrderDescription(getType(), getAuto(), getEth(), false);
}
