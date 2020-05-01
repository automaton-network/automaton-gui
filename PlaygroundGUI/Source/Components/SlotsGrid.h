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

class SlotsGrid : public Component,
                  public Timer {
 public:
  void paint(Graphics& g) override;
  void mouseMove(const MouseEvent& event) override;
  void mouseExit(const MouseEvent& event) override;
  void timerCallback() override;
  int getSlotIndex(const Point<float>& pos);

  virtual void updateContent();
  virtual Colour getSlotColour(int slotIndex, bool isHighlighted) = 0;
  virtual int getNumOfSlots() = 0;
  virtual Component* getPopupComponent(int slotIndex) = 0;

 private:
  int m_numOfSlotsPerSide = 0;
  int m_slotSize = 0;
  int m_highlightedSlot = -1;
  Component* m_popupComponent = nullptr;
};
