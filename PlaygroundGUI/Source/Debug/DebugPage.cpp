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

DebugPage::DebugPage() {
  m_tasksModel = TasksManager::getInstance()->getTasksModel();
  m_tasksModel->addListener(this);
  m_tasksListBox = std::make_unique<ListBox>();
  m_tasksListBox->setRowHeight(30);
  m_tasksListBox->setModel(this);
  addAndMakeVisible(m_tasksListBox.get());
}

DebugPage::~DebugPage() {
}

void DebugPage::resized() {
  m_tasksListBox->setBounds(getLocalBounds());
}

void DebugPage::paint(Graphics &g) {
}

int DebugPage::getNumRows() {
  return m_tasksModel->size();
}

void DebugPage::paintListBoxItem(int rowNumber, Graphics& g, int width, int height, bool rowIsSelected) {
  auto task = m_tasksModel->getAt(rowNumber);
  if (task == nullptr)
    return;

  g.setColour(Colours::white);
  g.drawRect(0, 0, width, height, 1);
  g.drawText(task->getTitle(), 10, 0, width, height, Justification::centredLeft);
}

void DebugPage::listBoxItemDoubleClicked(int row, const MouseEvent&) {
}
void DebugPage::modelChanged(AbstractListModelBase*) {
  m_tasksListBox->updateContent();
}
