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

class TaskComponent : public Component,
                      public AsyncTask::Listener{
 public:
  TaskComponent(TasksPanel* owner, AsyncTask::Ptr task) {
    addAndMakeVisible(m_titleLabel);
    addAndMakeVisible(m_messageLabel);
    m_owner = owner;
    m_task = task;
    m_task->addListener(this);
    m_titleLabel.setText(m_task->getThreadName(), NotificationType::dontSendNotification);
    m_messageLabel.setText(m_task->getStatusMessage(), NotificationType::dontSendNotification);
  }

  ~TaskComponent() {
    m_task->removeListener(this);
  }

  void taskFinished(AsyncTask::Ptr task) override {
  }

  void taskMessageChanged(AsyncTask::Ptr task) override {
    m_messageLabel.setText(task->getStatusMessage(), NotificationType::dontSendNotification);
    repaint();
  }

  void taskProgressChanged(AsyncTask::Ptr task) override {
    m_owner->setProgress(task->getProgress());
  }

  AsyncTask::Ptr getTask() {
    return m_task;
  }

  void resized() override {
    auto bounds = getLocalBounds();
    m_titleLabel.setBounds(bounds.removeFromTop(getHeight()/2));
    m_messageLabel.setBounds(bounds);
  }

  void paint(Graphics& g) override {
    g.fillAll(Colours::grey.withAlpha(0.1f));
  }

 private:
  Label m_titleLabel;
  Label m_messageLabel;
  TasksPanel* m_owner = nullptr;
  AsyncTask::Ptr m_task;
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

void TasksPanel::setProgress(double progress) {
  m_progress = progress;
}

void TasksPanel::resized() {
  auto bounds = getLocalBounds();

  for (auto comp : m_tasksComponents) {
    comp->setBounds(bounds.removeFromTop(100));
  }
}

void TasksPanel::paint(Graphics& g) {
  g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
}

void TasksPanel::modelChanged(AbstractListModelBase*) {
  Array<TaskComponent*> m_newTasksList;

  auto getComponentByTask = [&](AsyncTask::Ptr task) -> TaskComponent* {
    for (int i = 0; i < m_tasksComponents.size(); ++i) {
      auto comp = m_tasksComponents[i];
      if (comp->getTask() == task)
        return m_tasksComponents.removeAndReturn(i);
    }
    return nullptr;
  };

  for (int i = 0; i < m_tasksProxyModel->size(); ++i) {
    if (auto task = m_tasksProxyModel->getAt(i)) {
      auto comp = getComponentByTask(task);
      if (comp == nullptr)
        comp = new TaskComponent(this, task);

      addAndMakeVisible(comp);
      m_newTasksList.add(comp);
    }
  }

  m_progressBar->setVisible(!m_newTasksList.isEmpty());

  m_tasksComponents.clearQuick(true);
  m_tasksComponents.addArray(m_newTasksList);
  resized();
  repaint();
}

