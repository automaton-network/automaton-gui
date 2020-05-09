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

#include "DebugPage.h"

class TaskLogComponent : public Component, public Button::Listener {
 public:
  TaskLogComponent(): m_backBtn("Back") {
    m_backBtn.addListener(this);
    addAndMakeVisible(m_backBtn);

    m_logEditor.setMultiLine(true);
    m_logEditor.setScrollbarsShown(true);
    m_logEditor.setReadOnly(true);
    addAndMakeVisible(m_logEditor);
  }

  void paint(Graphics& g) override {
    g.fillAll(Colour(0xff404040));
  }

  void resized() override {
    auto bounds = getLocalBounds();
    m_backBtn.setBounds(bounds.removeFromBottom(30).removeFromLeft(100));
    m_logEditor.setBounds(bounds);
  }

  void buttonClicked(Button* button) override {
    if (button == &m_backBtn)
      setVisible(false);
  }

  TextButton m_backBtn;
  TextEditor m_logEditor;
};

DebugPage::DebugPage() {
  m_tasksModel = TasksManager::getInstance()->getTasksModel();
  m_tasksModel->addListener(this);
  m_tasksListBox = std::make_unique<ListBox>();
  m_tasksListBox->setRowHeight(30);
  m_tasksListBox->setModel(this);
  addAndMakeVisible(m_tasksListBox.get());

  m_taskLogComponent = std::make_unique<TaskLogComponent>();
  addChildComponent(m_taskLogComponent.get());
}

DebugPage::~DebugPage() {
}

void DebugPage::resized() {
  m_tasksListBox->setBounds(getLocalBounds());
  m_taskLogComponent->setBounds(getLocalBounds());
}

void DebugPage::paint(Graphics &g) {
}

int DebugPage::getNumRows() {
  return m_tasksModel->size();
}

void DebugPage::paintListBoxItem(int rowNumber, Graphics& g, int width, int height, bool rowIsSelected) {
  auto task = m_tasksModel->getAt(m_tasksModel->size() - rowNumber - 1);
  if (task == nullptr)
    return;

  g.setColour(Colours::white);
  g.drawRect(0, 0, width, height, 1);
  g.drawText("#" + String(task->getTaskId()) + "  " + task->getTitle(),
             10, 0, width, height, Justification::centredLeft);
}

void DebugPage::listBoxItemDoubleClicked(int rowNumber, const MouseEvent&) {
  auto task = m_tasksModel->getAt(m_tasksModel->size() - rowNumber - 1);
  m_taskLogComponent->m_logEditor.setText(task->getTaskLog().joinIntoString("\n--------------\n"),
                                          NotificationType::dontSendNotification);
  m_taskLogComponent->m_logEditor.setCaretPosition(0);
  m_taskLogComponent->setVisible(true);
}
void DebugPage::modelChanged(AbstractListModelBase*) {
  m_tasksListBox->updateContent();
}
