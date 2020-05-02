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


class Order {
 public:
  enum class Type {
    None = 0
    , Buy
    , Sell
    , Auction
  };

  using Ptr = std::shared_ptr<Order>;

  static String getOrderDescription(Type orderType,
                                    const String& amountAUTO, const String& amountETH,
                                    bool convertFromWei = false);

  Order(uint64 id, const String& jsonString);

  uint64 getId() const noexcept { return m_id; }
  Type getType() const noexcept { return m_type; }
  String getAuto() const noexcept   { return m_auto; }
  String getEth() const noexcept    { return m_eth; }
  String getPrice() const noexcept  { return m_price; }
  String getOwner() const noexcept  { return m_owner; }

  String getDescription() const noexcept;

  void setId(uint64 id) { m_id = id; }

 private:
  uint64 m_id;

  Type m_type = Type::None;

  String m_auto;
  String m_eth;
  String m_price;
  String m_owner;
};
