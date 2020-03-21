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

#include "OrdersManager.h"
#include "OrdersModel.h"


OrdersManager::OrdersManager()
{
  m_model = std::make_shared<OrdersModel>();
}

OrdersManager::~OrdersManager()
{
  clearSingletonInstance();
}

std::shared_ptr<OrdersModel> OrdersManager::getModel()
{
  return m_model;
}

JUCE_IMPLEMENT_SINGLETON(OrdersManager);
