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

class FlatProgressBarLookAndFeel : public LookAndFeel_V4 {
 public:
  void drawProgressBar(Graphics& g, ProgressBar& progressBar,
                       int width, int height,
                       double progress,
                       const String& textToShow) override {
    auto background = progressBar.findColour(ProgressBar::backgroundColourId);
    auto foreground = progressBar.findColour(ProgressBar::foregroundColourId);

    auto barBounds = progressBar.getLocalBounds().toFloat();

    g.setColour(background);
    Path p;
    p.addRectangle(barBounds);
    g.reduceClipRegion(p);

    if (progress >= 0.0f && progress <= 1.0f) {
        barBounds.setWidth(barBounds.getWidth() * static_cast<float>(progress));
        g.setColour(foreground);
        g.fillRect(barBounds);
    }
  }
};

class TaskStatusBar : public Component
                    , public AsyncUpdater
                    , public AsyncTask::Listener {
 public:
  TaskStatusBar()
      : m_numTasks(0)
      , m_progress(0.0)
      , m_statusMessage("")
      , m_progressBar(std::make_unique<ProgressBar>(m_progress)) {
    m_progressBar->setLookAndFeel(&m_progressBarLookAndFeel.get());
    m_progressBar->setColour(ProgressBar::backgroundColourId, Colours::transparentBlack);
    m_progressBar->setColour(ProgressBar::foregroundColourId, Colour(0xFF00BCD4));
    addAndMakeVisible(m_progressBar.get());
  }

  ~TaskStatusBar() {
    m_progressBar->setLookAndFeel(nullptr);
  }

  void paint(Graphics& g) override {
    auto bounds = getLocalBounds().reduced(10, 5);

    // Draw number of active tasks
    if (m_numTasks) {
      g.setColour(Colour(0xFF4CAF50));
      g.drawSingleLineText(String(m_numTasks),
                           bounds.getX(), bounds.getBottom(),
                           Justification::horizontallyCentred);
      bounds.removeFromLeft(30);
    }

    g.setColour(Colours::white);
    g.drawSingleLineText(m_statusMessage, bounds.getX(), bounds.getBottom());
  }

  void resized() override {
    auto bounds = getLocalBounds();
    m_progressBar->setBounds(bounds.removeFromBottom(3));
  }

  ProgressBar* getProgressBar() const noexcept {
    return m_progressBar.get();
  }

  void setProgress(double progress) {
    m_progress = progress;
    triggerAsyncUpdate();
  }

  void setOwner(Component* owner) {
    m_owner = owner;
  }

  void setNumTasks(uint32 numTasks) {
    m_numTasks = numTasks;
    triggerAsyncUpdate();
  }

  void setStatusMessage(const String& statusMessage) {
    m_statusMessage = statusMessage;
    triggerAsyncUpdate();
  }

  void handleAsyncUpdate() override {
    repaint();
  }

  void taskMessageChanged(AsyncTask::Ptr task) override {
    setStatusMessage(task->getStatusMessage());
  }

  void taskProgressChanged(AsyncTask::Ptr task) override {
    setProgress(task->getProgress());
  }

  void taskFinished(AsyncTask::Ptr task) override {
    task->removeListener(this);
  }

  void mouseUp(const MouseEvent& e) override {
    if (m_owner)
      m_owner->setVisible(!m_owner->isVisible());
  }

 private:
  uint32 m_numTasks;
  double m_progress;

  String m_statusMessage;

  Component* m_owner = nullptr;

  std::unique_ptr<ProgressBar> m_progressBar;
  SharedResourcePointer<FlatProgressBarLookAndFeel> m_progressBarLookAndFeel;
};

class TaskComponent : public Component
                    , public AsyncUpdater
                    , public AsyncTask::Listener {
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
    triggerAsyncUpdate();
  }

  void taskProgressChanged(AsyncTask::Ptr task) override {
    triggerAsyncUpdate();
  }

  void handleAsyncUpdate() override {
    m_messageLabel.setText(m_task->getStatusMessage(), NotificationType::dontSendNotification);
    repaint();
  }

  AsyncTask::Ptr getTask() {
    return m_task;
  }

  void resized() override {
    auto bounds = getLocalBounds();
    m_titleLabel.setBounds(bounds.removeFromTop(getHeight() / 2));
    m_messageLabel.setBounds(bounds);
  }

  void paint(Graphics& g) override {
    g.fillAll(Colours::grey.withAlpha(0.1f));

    // Draw progress
    g.setColour(Colour(0xFF00BCD4).withAlpha(0.2f));
    g.fillRect(getLocalBounds().withWidth(getWidth() * static_cast<float>(m_task->getProgress())));

    g.setColour(Colours::white);
    g.fillRect(getLocalBounds().removeFromBottom(2));
  }

 private:
  Label m_titleLabel;
  Label m_messageLabel;
  TasksPanel* m_owner = nullptr;
  AsyncTask::Ptr m_task;
};

TasksPanel::TasksPanel()
    : m_tasksProxyModel(std::make_shared<TasksProxyModel>())
    , m_statusBar(std::make_unique<TaskStatusBar>()) {
  m_tasksProxyModel->setModel(TasksManager::getInstance()->getModel());
  m_tasksProxyModel->addListener(this);

  m_statusBar->setOwner(this);
}

TasksPanel::~TasksPanel() {
}

Component* TasksPanel::getStatusBarComponent() const noexcept {
  return dynamic_cast<Component*>(m_statusBar.get());
}

void TasksPanel::resized() {
  auto bounds = getLocalBounds();

  for (auto comp : m_tasksComponents) {
    comp->setBounds(bounds.removeFromTop(60));
  }
}

void TasksPanel::paint(Graphics& g) {
  g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
  g.setColour(Colours::white);
  g.fillRect(getLocalBounds().removeFromRight(2));
  g.fillRect(getLocalBounds().removeFromBottom(2));
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
      task->addListener(m_statusBar.get());
    }
  }

  m_statusBar->setNumTasks(m_tasksProxyModel->size());
  m_statusBar->getProgressBar()->setVisible(!m_newTasksList.isEmpty());

  m_tasksComponents.clearQuick(true);
  m_tasksComponents.addArray(m_newTasksList);
  resized();
  repaint();
}

void TasksPanel::mouseUp(const MouseEvent& e) {
  if (e.originalComponent == this) {
    setVisible(false);
  }
}
