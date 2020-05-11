/*
 * Automaton Playground
 * Copyright (c) 2019 The Automaton Authors.
 * Copyright (c) 2019 The automaton.network Authors.
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

#include <Models/AbstractListModel.h>
#include <Login/Account.h>
#include "JuceHeader.h"
#include "AsyncTask.h"

class AsyncTaskModel : public AbstractListModel<AsyncTask::Ptr> {
 public:
  int size() const override;
  AsyncTask::Ptr getAt(int index) override;
  AsyncTask::Ptr& getReferenceAt(int index) override;
  void addItem(AsyncTask::Ptr item, NotificationType notification);
  void removeItem(AsyncTask* item, NotificationType notification);
  void clear(NotificationType notification);

 private:
  Array<AsyncTask::Ptr> m_items;
};

class TasksManager : public DeletedAtShutdown {
 public:
  TasksManager();
  ~TasksManager();

  static AsyncTask::Ptr launchTask(std::function<bool(AsyncTask*)> fun,
                                   std::function<void(AsyncTask*)> postAsyncAction,
                                   const String& title,
                                   Account::Ptr account = nullptr,
                                   bool isQueued = true);

  static bool launchTask(std::function<bool(TaskWithProgressWindow*)> fun,
                         std::function<void(TaskWithProgressWindow*)> postAction,
                         const String& title);

  void addTask(AsyncTask::Ptr task, bool isQueued);
  void runQueuedTask();
  std::shared_ptr<AsyncTaskModel> getActiveTasksModel();
  std::shared_ptr<AsyncTaskModel> getTasksModel();

  JUCE_DECLARE_SINGLETON(TasksManager, true)

 private:
  std::shared_ptr<AsyncTaskModel> m_activeTasksModel;
  std::shared_ptr<AsyncTaskModel> m_model;
  Array<AsyncTask::Ptr> m_queuedTasks;
  CriticalSection m_lock;
};
