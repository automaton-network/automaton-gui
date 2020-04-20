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
#include "../Models/AbstractListModel.h"

class TasksProxyModel;

class TasksPanel : public Component
                 , public AbstractListModelBase::Listener{
 public:
  TasksPanel();
  ~TasksPanel();

  ProgressBar* getProgressBar() const noexcept;

  // AbstractListModelBase::Listener
  void modelChanged(AbstractListModelBase* base) override;

 private:
  std::shared_ptr<TasksProxyModel> m_tasksProxyModel;
  std::unique_ptr<ProgressBar> m_progressBar;
  double m_progress;
};
