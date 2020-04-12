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

#include "TasksManager.h"

JUCE_IMPLEMENT_SINGLETON(TasksManager)

TasksManager::TasksManager() {
}

TasksManager::~TasksManager() {
  clearSingletonInstance();
}

void TasksManager::launchTask(std::function<bool(AsyncTask*)> fun,
                              std::function<void(AsyncTask*)> postAsyncAction,
                              const String& title) {
  TasksManager::getInstance()->addTask(new AsyncTask (fun, postAsyncAction, title));
}

bool TasksManager::launchTask(std::function<bool(TaskWithProgressWindow*)> fun,
                              std::function<void(TaskWithProgressWindow*)> postAction,
                              const String& title) {
  TaskWithProgressWindow task(fun, title);

  if (task.runThread()) {
    postAction(&task);

    auto& s = task.m_status;
    if (!s.is_ok()) {
      AlertWindow::showMessageBoxAsync(
          AlertWindow::WarningIcon,
          "ERROR",
          String("(") + String(s.code) + String(") :") + s.msg);
    }
  } else {
    AlertWindow::showMessageBoxAsync(
        AlertWindow::WarningIcon,
        "Canceled!",
        "Operation aborted.");
  }

  return task.m_status.is_ok();
}

void TasksManager::addTask(AsyncTask* task) {
  ScopedLock sl(m_lock);
  m_tasks.add(task);

  if (m_tasks.size() == 1)
    runQueuedTask();
}

void TasksManager::runQueuedTask() {
  if (auto task = m_tasks.getFirst()) {
    task->runThread([=](AsyncTask* task){
      m_tasks.removeObject(task, true);
      runQueuedTask();
    });
  }
}
