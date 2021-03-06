/*
 * Automaton Playground
 * Copyright (c) 2020 The Automaton Authors.
 * Copyright (c) 2020 The automaton.network Authors.
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

#include <JuceHeader.h>

class ProposalsGrid;
class ProposalsManager;

class ProposalsActionsPage : public Component {
 public:
  ProposalsActionsPage(ProposalsManager* proposalsManager);
  ~ProposalsActionsPage();

  void paint(Graphics& g) override;
  void resized() override;

 private:
  std::unique_ptr<Label> m_titleLabel;
  std::unique_ptr<ProposalsGrid> m_proposalsGrid;
  ProposalsManager* m_proposalsManager;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProposalsActionsPage)
};
