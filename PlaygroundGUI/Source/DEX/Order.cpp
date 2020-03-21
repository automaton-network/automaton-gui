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

using json = nlohmann::json;

Order::Order (const String& jsonString)
{
  json jsonData = json::parse (jsonString.toStdString());
  m_auto.parseString (jsonData.at (0).get<std::string>(), 10);
  m_eth.parseString (jsonData.at (1).get<std::string>(), 10);
  m_owner = jsonData.at (2).get<std::string>();
  m_type = static_cast<Order::Type> (std::stoul (jsonData.at (3).get<std::string>()));
}
