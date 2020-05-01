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

class AbstractListModelBase : public AsyncUpdater {
 public:
  class Listener {
   public:
    virtual ~Listener() {}
    virtual void modelChanged(AbstractListModelBase*) = 0;
  };

  virtual ~AbstractListModelBase() {
    cancelPendingUpdate();
  }

  virtual int size() const = 0;

  void handleAsyncUpdate() {
    m_listeners.call(&Listener::modelChanged, this);
  }

  virtual void notifyModelChanged(NotificationType notification) {
    if (notification != dontSendNotification)
      triggerAsyncUpdate();

    if (notification == sendNotification)
      handleUpdateNowIfNeeded();
  }

  void addListener(Listener* listener) {
    m_listeners.add(listener);
  }

  void removeListener(Listener* listener) {
    m_listeners.remove(listener);
  }

 private:
  ListenerList<Listener> m_listeners;
};

template<typename T>
class AbstractListModel : public AbstractListModelBase {
 public:
  virtual T getAt(int index) = 0;
  virtual T& getReferenceAt(int index) = 0;
  virtual int getIndexOf(const T&) { return -1; }
};
