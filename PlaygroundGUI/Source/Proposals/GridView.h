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

class GridView : public Component
{
public:
  GridView();
  ~GridView();

  void setSpacing (int horizontalSpacing, int verticalSpacing);
  void setMargins (int leftMargin, int topMargin, int rightMargin, int bottomMargin);
  void setCellMinimumWidth (int cellMinWidth);
  void setCellRatio (float cellRatio);

  void paint (Graphics&) override;
  void resized() override;

  void updateContent();
  virtual Component* refreshComponent (int index, Component* const componentToUpdate) = 0;
  virtual int size() = 0;

private:
  BorderSize<int> m_margins;
  float m_cellRatio;
  int m_numOfColumns;
  int m_numOfRows;
  int m_verticalSpacing;
  int m_horizontalSpacing;
  int m_cellMinWidth;
  int m_cellHeight;
  int m_cellWidth;

  Component m_gridContent;
  std::unique_ptr<Viewport> m_viewport;
  OwnedArray<Component> m_components;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GridView)
};
