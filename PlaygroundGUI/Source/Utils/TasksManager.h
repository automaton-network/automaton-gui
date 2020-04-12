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

#include "JuceHeader.h"
#include "AsyncTask.h"

class TasksManager : public DeletedAtShutdown {
 public:
  TasksManager();
  ~TasksManager();

  static void launchTask(std::function<bool(AsyncTask*)> fun,
                         std::function<void(AsyncTask*)> postAsyncAction,
                         const String& title);

  static bool launchTask(std::function<bool(TaskWithProgressWindow*)> fun,
                         std::function<void(TaskWithProgressWindow*)> postAction,
                         const String& title);

  void addTask(AsyncTask* task);
  void runQueuedTask();

  JUCE_DECLARE_SINGLETON(TasksManager, true)

 private:
  OwnedArray<AsyncTask> m_tasks;
  CriticalSection m_lock;
};
