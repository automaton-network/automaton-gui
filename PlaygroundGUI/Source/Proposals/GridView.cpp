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

#include <JuceHeader.h>
#include "GridView.h"

//==============================================================================
GridView::GridView():
  m_verticalSpacing(35),
  m_horizontalSpacing(25),
  m_cellMinWidth(500),
  m_cellRatio(2.0),
  m_margins(50, 50, 50, 50) {
  m_viewport = std::make_unique<Viewport>();
  m_viewport->setScrollBarsShown(true, false, true);
  m_viewport->setViewedComponent(&m_gridContent, false);
  addAndMakeVisible(m_viewport.get());
}

GridView::~GridView() {
}

void GridView::setSpacing(int horizontalSpacing, int verticalSpacing) {
  m_horizontalSpacing = horizontalSpacing;
  m_verticalSpacing = verticalSpacing;
}

void GridView::setMargins(int leftMargin, int topMargin, int rightMargin, int bottomMargin) {
  m_margins = BorderSize<int>(topMargin, leftMargin, bottomMargin, rightMargin);
}

void GridView::setCellMinimumWidth(int cellMinWidth) {
  m_cellMinWidth = cellMinWidth;
}

void GridView::setCellRatio(float cellRatio) {
  m_cellRatio = cellRatio;
}

void GridView::paint(Graphics& g) {
}

void GridView::resized() {
  const int numOfItems = size();
  const int width = getWidth() - m_margins.getLeftAndRight();

  m_numOfColumns = (width + m_horizontalSpacing) / (m_cellMinWidth + m_horizontalSpacing);
  if (m_numOfColumns == 0)
    return;

  m_cellWidth  = static_cast<int>((width - (m_numOfColumns - 1) * m_horizontalSpacing)
                                    / static_cast<float>(m_numOfColumns));
  m_cellHeight = static_cast<int>(m_cellWidth / m_cellRatio);
  m_numOfRows  = static_cast<int>(std::ceil(numOfItems/ static_cast<float>(m_numOfColumns)));

  const int totalHeight = m_numOfRows * m_cellHeight
                            + (m_numOfRows - 1) * m_verticalSpacing
                            + m_margins.getTopAndBottom();
  m_gridContent.setSize(getWidth(), totalHeight);

  auto gridArea = m_margins.subtractedFrom(m_gridContent.getLocalBounds());

  int index = 0;
  for (int row = 0; row < m_numOfRows; ++row) {
    auto rowArea = gridArea.removeFromTop(m_cellHeight);

    for (int column = 0; column < m_numOfColumns && index < m_components.size(); ++column) {
      if (auto comp = m_components[index])
        comp->setBounds(rowArea.removeFromLeft(m_cellWidth));

      rowArea.removeFromLeft(m_horizontalSpacing);
      ++index;
    }

    gridArea.removeFromTop(m_verticalSpacing);
  }

  m_viewport->setBounds(getLocalBounds());
}

void GridView::updateContent() {
  const auto diffSize = m_components.size() - size();
  if (diffSize > 0)
    m_components.removeRange(m_components.size() - 1 - diffSize, diffSize, true);

  for (int i = 0; i < size(); ++i) {
    const auto component = m_components[i];
    const auto item = refreshComponent(i, component);
    m_gridContent.addAndMakeVisible(item);

    if (component == nullptr)
      m_components.add(item);
  }
}
