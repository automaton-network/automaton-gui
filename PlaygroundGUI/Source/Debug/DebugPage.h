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

#include <Utils/TasksManager.h>
#include "JuceHeader.h"

class TaskLogComponent;

class DebugPage : public Component,
                  public ListBoxModel,
                  public AbstractListModelBase::Listener {
 public:
  DebugPage();
  ~DebugPage();

  void resized() override;
  void paint(Graphics& g) override;
  int getNumRows() override;
  void paintListBoxItem(int rowNumber, Graphics& g, int width, int height, bool rowIsSelected) override;
  void listBoxItemDoubleClicked(int row, const MouseEvent&) override;
  void modelChanged(AbstractListModelBase*) override;

 private:
  std::unique_ptr<ListBox> m_tasksListBox;
  std::unique_ptr<TaskLogComponent> m_taskLogComponent;
  std::shared_ptr<AsyncTaskModel> m_tasksModel;
};
