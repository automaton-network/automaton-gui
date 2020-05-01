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
    m_slotSize = getWidth() / m_numOfSlotsPerSide;

    int slotIndex = 0;
    for (int i = 0; i < m_numOfSlotsPerSide; ++i) {
      auto rowBounds = bounds.removeFromTop(m_slotSize);
      for (int j = 0; j < m_numOfSlotsPerSide; ++j) {
        const auto slotRect = rowBounds.removeFromLeft(m_slotSize);
        g.setColour(getSlotColour(slotIndex, m_highlightedSlot == slotIndex));
        g.fillRect(slotRect);
        g.setColour(Colours::black);
        g.drawRect(slotRect);
        ++slotIndex;
      }
    }
  }
}

void SlotsGrid::mouseMove(const MouseEvent& event) {
  const auto highlightedSlot = getSlotIndex(event.position);
  if (m_highlightedSlot != highlightedSlot) {
    startTimer(500);
    m_highlightedSlot = highlightedSlot;

    if (m_popupComponent != nullptr)
      m_popupComponent->removeFromDesktop();

    m_popupComponent = getPopupComponent(m_highlightedSlot);
  }

  repaint();
}

void SlotsGrid::mouseExit(const MouseEvent& event) {
  mouseMove(event);
}

void SlotsGrid::timerCallback() {
  stopTimer();
  if (m_popupComponent != nullptr) {
    m_popupComponent->setTopLeftPosition(Desktop::getMousePosition());
    m_popupComponent->setAlwaysOnTop(true);
    m_popupComponent->addToDesktop(ComponentPeer::StyleFlags::windowIsTemporary);
  }
}

int SlotsGrid::getSlotIndex(const Point<float>& pos) {
  if (m_slotSize <= 0)
    return -1;

  const int column = static_cast<int>(std::ceil(pos.x / m_slotSize)) - 1;
  const int row = static_cast<int>(std::ceil(pos.y / m_slotSize)) - 1;

  if (isPositiveAndBelow(column, m_numOfSlotsPerSide) && isPositiveAndBelow (row, m_numOfSlotsPerSide))
    return row * m_numOfSlotsPerSide + column;

  return -1;
}

void SlotsGrid::updateContent() {
  m_numOfSlotsPerSide = static_cast<int>(std::ceil(std::sqrt(getNumOfSlots())));
  resized();
  repaint();
}
