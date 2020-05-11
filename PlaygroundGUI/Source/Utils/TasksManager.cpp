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

TasksManager::TasksManager() : m_activeTasksModel(std::make_shared<AsyncTaskModel>()),
                               m_model(std::make_shared<AsyncTaskModel>()) {
}

TasksManager::~TasksManager() {
  clearSingletonInstance();
}

AsyncTask::Ptr TasksManager::launchTask(std::function<bool(AsyncTask*)> fun,
                                        std::function<void(AsyncTask*)> postAsyncAction,
                                        const String& title,
                                        Account::Ptr account,
                                        bool isQueued) {
  auto task = std::make_shared<AsyncTask> (fun, postAsyncAction, title, account ? account->getAccountId() : 0);
  TasksManager::getInstance()->addTask(task, isQueued);
  return task;
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

void TasksManager::addTask(AsyncTask::Ptr task, bool isQueued) {
  ScopedLock sl(m_lock);
  m_model->addItem(task, NotificationType::sendNotification);
  m_activeTasksModel->addItem(task, NotificationType::sendNotification);

  if (isQueued) {
    m_queuedTasks.add(task);
    if (m_queuedTasks.size() == 1)
      runQueuedTask();
  } else {
    task->runThread([=](AsyncTask* task){
      m_activeTasksModel->removeItem(task, NotificationType::sendNotification);
    });
  }
}

void TasksManager::runQueuedTask() {
  if (auto task = m_queuedTasks.getFirst()) {
    task->runThread([=](AsyncTask* task){
      m_activeTasksModel->removeItem(task, NotificationType::sendNotification);
      m_queuedTasks.removeIf([&](AsyncTask::Ptr itemPtr){return itemPtr.get() == task;});
      runQueuedTask();
    });
  }
}

std::shared_ptr<AsyncTaskModel> TasksManager::getActiveTasksModel() {
  return m_activeTasksModel;
}

std::shared_ptr<AsyncTaskModel> TasksManager::getTasksModel() {
  return m_model;
}
