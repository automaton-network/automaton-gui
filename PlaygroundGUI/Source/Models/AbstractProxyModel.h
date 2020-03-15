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

#include "AbstractListModel.h"

template<typename T>
class AbstractProxyModel : public AbstractListModel<T>,
  private AbstractListModelBase::Listener
{
public:
  AbstractProxyModel()
  {
  }

  virtual ~AbstractProxyModel()
  {
    if (m_model != nullptr)
      m_model->removeListener (this);
  }

  void setModel (std::shared_ptr<AbstractListModel<T>> model)
  {
    if (m_model != nullptr)
      m_model->removeListener (this);

    m_model = model;

    if (m_model != nullptr)
      m_model->addListener (this);

    modelChanged (m_model.get());
  }

  virtual void filterChanged()
  {
    if (auto model = m_model)
    {
      m_proxyArray.clearQuick();
      m_proxyArray.ensureStorageAllocated (model->size());

      const auto size = model->size();

      for (int i = 0; i < size; ++i)
      {
        if (isAccept (model->getReferenceAt (i)))
          m_proxyArray.add (i);
      }

      if (withSorting())
        m_proxyArray.sort (*this, true);
      this->notifyModelChanged();
    }
  }

  int size() override
  {
    return m_proxyArray.size();
  }

  virtual T getAt (int index) override
  {
    return m_model->getAt (m_proxyArray[index]);
  }

  virtual T& getReferenceAt (int index) override
  {
    return m_model->getReferenceAt (m_proxyArray[index]);
  }

  virtual int getIndexOf (const T& item) override
  {
    auto index = m_model->getIndexOf (item);
    if (index >= 0)
      return m_proxyArray.indexOf (index);

    return -1;
  }

  int compareElements (const int first, const int second) const
  {
    return compareData (m_model->getAt (m_proxyArray[first]),
                        m_model->getAt (m_proxyArray[second]));
  }

private:
  void modelChanged (AbstractListModelBase*) override
  {
    if (m_model != nullptr)
    {
      filterChanged();
    }
    else
    {
      m_proxyArray.clear();
      this->notifyModelChanged();
    }
  }

  Array<int> m_proxyArray;
  std::shared_ptr<AbstractListModel<T>> m_model;

protected:
  virtual bool isAccept (const T& index) = 0;
  virtual bool withSorting() = 0;
  virtual int compareData (const T& first, const T& second) const = 0;
};
