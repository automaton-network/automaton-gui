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

#include "../Models/AbstractListModel.h"
#include "../Models/AbstractProxyModel.h"
#include "Order.h"


class OrdersModel : public AbstractListModel<Order::Ptr> {
 public:
  int size() const override;
  Order::Ptr getAt(int index) override;
  Order::Ptr& getReferenceAt(int index) override;

  void addItem(Order::Ptr item, bool sendNotification = true);
  void addItems(Array<Order::Ptr> items, bool sendNotification = true);
  void clear(bool sendNotification = true);

 private:
  Array<Order::Ptr> m_items;
};

enum class OrderFilter {
  All = 1
  , Buy
  , Sell
  , Auction
};

class OrdersProxyModel : public AbstractProxyModel<Order::Ptr> {
 public:
  void setFilter(OrderFilter filter);
  void setSorter(std::function<int(Order*, Order*)> sorter);

 protected:
  bool isAccept(const Order::Ptr& item) override;
  bool withSorting() override;
  int compareData(const Order::Ptr& first, const Order::Ptr& second) const override;

 private:
  OrderFilter m_filter = OrderFilter::All;
  std::function<int(Order*, Order*)> m_sorterFun;
};
