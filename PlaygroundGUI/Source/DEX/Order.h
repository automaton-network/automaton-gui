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

  Order(const String& jsonString);

  BigInteger getAuto() const { return m_auto; }
  BigInteger getEth() const { return m_eth; }
  const String& getOwner() const { return m_owner; }
  Type getType() const { return m_type; }

 private:
  BigInteger m_auto = 0;
  BigInteger m_eth = 0;
  String m_owner;
  Type m_type = Type::None;
};
