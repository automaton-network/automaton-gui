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

#include "../Models/AbstractProxyModel.h"
#include "Proposal.h"

class ProposalsModel : public AbstractListModel<Proposal::Ptr>
{
public:
  int size () override;
  Proposal::Ptr getAt (int index) override;
  Proposal::Ptr& getReferenceAt (int index) override;

  void addItem (Proposal::Ptr item, bool sendNotification = true);
  void clear (bool sendNotification = true);

private:
  Array<Proposal::Ptr> m_items;
};

enum class ProposalFilter {
  All = 1
  , PayingGas
  , Approved
  , Accepted
  , Rejected
  , Contested
  , Completed
};

class ProposalsProxyModel : public AbstractProxyModel<Proposal::Ptr>
{
public:
  void setFilter (ProposalFilter filter);
  void setSorter (std::function<int(Proposal*, Proposal*)> sorter);

protected:
  bool isAccept (const Proposal::Ptr& item) override;
  bool withSorting() override;
  int compareData (const Proposal::Ptr& first, const Proposal::Ptr& second) const override;

private:
  ProposalFilter m_filter = ProposalFilter::All;
  std::function<int(Proposal*, Proposal*)> m_sorterFun;
};
