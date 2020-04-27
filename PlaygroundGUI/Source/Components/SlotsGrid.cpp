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

#include "SlotsGrid.h"
#include <cmath>

void SlotsGrid::paint(Graphics& g) {
  if (m_numOfSlotsPerSide != 0) {
    auto bounds = getLocalBounds();
    const int slotSize = bounds.getWidth() / m_numOfSlotsPerSide;

    int slotIndex = 0;
    for (int i = 0; i < m_numOfSlotsPerSide; ++i) {
      auto rowBounds = bounds.removeFromTop(slotSize);
      for (int j = 0; j < m_numOfSlotsPerSide; ++j) {
        const auto slotRect = rowBounds.removeFromLeft(slotSize);
        g.setColour(getSlotColour(slotIndex));
        g.fillRect(slotRect);
        g.setColour(Colours::black);
        g.drawRect(slotRect);
        ++slotIndex;
      }
    }
  }
}

void SlotsGrid::updateContent() {
  m_numOfSlotsPerSide = static_cast<int>(std::ceil(std::sqrt(getNumOfSlots())));
  resized();
  repaint();
}
