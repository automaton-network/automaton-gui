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

int AsyncTaskModel::size() const {
  return m_items.size();
}
AsyncTask::Ptr AsyncTaskModel::getAt(int index) {
  return m_items[index];
}
AsyncTask::Ptr& AsyncTaskModel::getReferenceAt(int index) {
  return m_items.getReference(index);
}
void AsyncTaskModel::addItem(AsyncTask::Ptr item, NotificationType notification) {
  m_items.add(item);
  notifyModelChanged(notification);
}
void AsyncTaskModel::removeItem(AsyncTask* item, NotificationType notification) {
  m_items.removeIf([&](AsyncTask::Ptr itemPtr){return itemPtr.get() == item;});
  notifyModelChanged(notification);
}

JUCE_IMPLEMENT_SINGLETON(TasksManager)

TasksManager::TasksManager() : m_model(std::make_shared<AsyncTaskModel>()) {
}

TasksManager::~TasksManager() {
  clearSingletonInstance();
}

void TasksManager::launchTask(std::function<bool(AsyncTask*)> fun,
                              std::function<void(AsyncTask*)> postAsyncAction,
                              const String& title,
                              Account::Ptr account) {
  TasksManager::getInstance()->addTask(std::make_shared<AsyncTask> (fun, postAsyncAction,
      title, account->getAccountId()));
}

bool TasksManager::launchTask(std::function<bool(TaskWithProgressWindow*)> fun,
                              std::function<void(TaskWithProgressWindow*)> postAction,
                              const String& title) {
  TaskWithProgressWindow task(fun, title);

  if (task.runThread()) {
    if (postAction != nullptr)
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

void TasksManager::addTask(AsyncTask::Ptr task) {
  ScopedLock sl(m_lock);
  m_model->addItem(task, NotificationType::sendNotification);

  if (m_model->size() == 1)
    runQueuedTask();
}

void TasksManager::runQueuedTask() {
  if (auto task = m_model->getAt(0)) {
    task->runThread([=](AsyncTask* task){
      m_model->removeItem(task, NotificationType::sendNotification);
      runQueuedTask();
    });
  }
}

std::shared_ptr<AsyncTaskModel> TasksManager::getModel() {
  return m_model;
}
