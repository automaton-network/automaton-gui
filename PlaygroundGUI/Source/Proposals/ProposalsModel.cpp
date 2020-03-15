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

void ProposalsModel::addItem (Proposal::Ptr item, bool sendNotification)
{
  m_items.add (item);

  if (sendNotification)
    notifyModelChanged();
}

void ProposalsModel::clear (bool sendNotification)
{
  m_items.clearQuick();
  if (sendNotification)
    notifyModelChanged();
}

bool ProposalsProxyModel::isAccept (const Proposal::Ptr& item)
{
  switch (m_filter)
  {
    case ProposalFilter::All:
      return true;
    case ProposalFilter::PayingGas:
      return item->getStatus() == Proposal::Status::Uninitialized;
    case ProposalFilter::Approved:
      return true; //TODO
    case ProposalFilter::Accepted:
      return item->getStatus() == Proposal::Status::Accepted;
    case ProposalFilter::Rejected:
      return item->getStatus() == Proposal::Status::Rejected;
    case ProposalFilter::Contested:
      return item->getStatus() == Proposal::Status::Contested;
    case ProposalFilter::Completed:
      return item->getStatus() == Proposal::Status::Completed;
  }
}

bool ProposalsProxyModel::withSorting()
{
  return m_sorterFun != nullptr;
}

void ProposalsProxyModel::setFilter (ProposalFilter filter)
{
  m_filter = filter;
  filterChanged();
}

void ProposalsProxyModel::setSorter (std::function<int(Proposal*, Proposal*)> sorter)
{
  m_sorterFun = sorter;
  filterChanged();
}

int ProposalsProxyModel::compareData (const Proposal::Ptr& first, const Proposal::Ptr& second) const
{
  return m_sorterFun (first.get(), second.get());
}
