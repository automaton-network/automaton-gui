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

#include "OrdersModel.h"


int OrdersModel::size() const {
  return m_items.size();
}

Order::Ptr OrdersModel::getAt(int index) {
  return m_items[index];
}

Order::Ptr& OrdersModel::getReferenceAt(int index) {
  return m_items.getReference(index);
}

void OrdersModel::addItem(Order::Ptr item, bool sendNotification) {
  m_items.add(item);

  if (sendNotification)
    notifyModelChanged();
}

void OrdersModel::addItems(Array<Order::Ptr> items, bool sendNotification) {
  m_items.addArray(items);

  if (sendNotification)
    notifyModelChanged();
}

void OrdersModel::clear(bool sendNotification) {
  m_items.clearQuick();
  if (sendNotification)
    notifyModelChanged();
}

bool OrdersProxyModel::isAccept(const Order::Ptr& item) {
  switch (m_filter) {
    case OrderFilter::All:
      return true;
    case OrderFilter::Buy:
      return item->getType() == Order::Type::Buy;
    case OrderFilter::Sell:
      return item->getType() == Order::Type::Sell;
    case OrderFilter::Auction:
      return item->getType() == Order::Type::Auction;
  }
}

bool OrdersProxyModel::withSorting() {
  return m_sorterFun != nullptr;
}

void OrdersProxyModel::setFilter(OrderFilter filter) {
  m_filter = filter;
  filterChanged();
}

void OrdersProxyModel::setSorter(std::function<int(Order*, Order*)> sorter) {
  m_sorterFun = sorter;
  filterChanged();
}

int OrdersProxyModel::compareData(const Order::Ptr& first, const Order::Ptr& second) const {
  return m_sorterFun(first.get(), second.get());
}
