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

#include <Utils/AsyncTask.h>
#include <Models/AbstractProxyModel.h>
#include "TasksPanel.h"
#include "TasksManager.h"

class TasksProxyModel : public AbstractProxyModel<AsyncTask::Ptr> {
 protected:
  bool isAccept(const AsyncTask::Ptr& item) override {
    return item->getOwnerId() == m_currentOwnerId;
  }

  bool withSorting() override {
    return false;
  }

  int compareData(const AsyncTask::Ptr& first, const AsyncTask::Ptr& second) const override {
    return 0;
  }

  String m_currentOwnerId;
};


TasksPanel::TasksPanel() {
  m_progressBar = std::make_unique<ProgressBar>(m_progress);
  m_tasksProxyModel = std::make_shared<TasksProxyModel>();
  m_tasksProxyModel->setModel(TasksManager::getInstance()->getModel());
  m_tasksProxyModel->addListener(this);
}

TasksPanel::~TasksPanel() {
}

ProgressBar* TasksPanel::getProgressBar() const noexcept {
  return m_progressBar.get();
}

void TasksPanel::modelChanged(AbstractListModelBase* base) {
}

