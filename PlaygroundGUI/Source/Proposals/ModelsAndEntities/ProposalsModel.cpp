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

#include "ProposalsModel.h"

int ProposalsModel::size()
{
  return m_items.size();
}

Proposal::Ptr ProposalsModel::getAt (int index)
{
  return m_items[index];
}

Proposal::Ptr& ProposalsModel::getReferenceAt (int index)
{
  return m_items.getReference (index);
}

void ProposalsModel::addItem (Proposal::Ptr item)
{
  m_items.add (item);
  notifyModelChanged();
}

bool ProposalsProxyModel::isAccept (int index)
{
  return true;
}

int ProposalsProxyModel::compareData (const Proposal::Ptr& first, const Proposal::Ptr& second) const
{
  return 0;
}
