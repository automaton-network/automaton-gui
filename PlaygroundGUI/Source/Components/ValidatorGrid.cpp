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

#include "ValidatorGrid.h"

static Colour HSV(double h, double s, double v) {
  double hh, p, q, t, ff;
  int64 i;
  double r, g, b;

  if (s <= 0.0) {
    r = v;
    g = v;
    b = v;
    return Colour(uint8(r*255), uint8(g*255), uint8(b*255));
  }

  hh = h;
  if (hh >= 360.0) hh = 0.0;
  hh /= 60.0;
  i = static_cast<int64>(hh);
  ff = hh - i;
  p = v * (1.0 - s);
  q = v * (1.0 - (s * ff));
  t = v * (1.0 - (s * (1.0 - ff)));

  switch (i) {
  case 0:
    r = v;
    g = t;
    b = p;
    break;
  case 1:
    r = q;
    g = v;
    b = p;
    break;
  case 2:
    r = p;
    g = v;
    b = t;
    break;
  case 3:
    r = p;
    g = q;
    b = v;
    break;
  case 4:
    r = t;
    g = p;
    b = v;
    break;
  case 5:
  default:
    r = v;
    g = p;
    b = q;
    break;
  }
  return Colour(uint8(r * 255), uint8(g * 255), uint8(b * 255));
}

ValidatorGrid::ValidatorGrid(std::shared_ptr<SlotsModelInterface> model, const std::string& owner,
  bool allow_text_over_slots):
  model(model),
  owner(owner),
  allowed_text_over_slots(allow_text_over_slots) {
  if (model == nullptr) {
    throw std::invalid_argument("Model is NULL!");
  }

  slots_number = model->get_slots_number();
  slots_per_side = std::ceil(std::sqrt(slots_number));
  slots.resize(slots_number);
}

ValidatorGrid::~ValidatorGrid() {}

void ValidatorGrid::paint(Graphics& g) {
  if (slot_size < 1) {
    return;
  }

  g.setColour(Colours::white);
  g.drawRect(Rectangle<int>(0, 0, component_side_size_px + 2, component_side_size_px + 2));

  for (uint32_t i = 0; i < slots_per_side; ++i) {
    for (uint32_t j = 0; j < slots_per_side; ++j) {
      uint32_t slot_index = i * slots_per_side + j;
      Rectangle<int> rect(1 + gap + (j * (slot_size + gap)), 1 + gap + (i * (slot_size + gap)), slot_size, slot_size);
      if (slot_index < slots_number) {
        Colour col;
        if (slots[slot_index].bits == 0) {
          col = Colours::black;
        } else {
          double lb;
          if (max_leading_bits != min_leading_bits) {
            lb = 1.0 * (slots[slot_index].bits - min_leading_bits) / (max_leading_bits - min_leading_bits);
          } else {
            lb = 1.;
          }
          col = HSV(slots[slot_index].is_mine ? 30 : 200, 1.0 - lb, 0.5 + 0.4 * lb);
        }
        g.setColour(col);
        g.fillRect(rect);
        if (slots[slot_index].is_mine) {
          g.setColour(Colours::red);
          g.drawRect(rect);
        }
        if (allowed_text_over_slots && slots[slot_index].bits) {
          g.setColour(col.contrasting(0.7));
          g.drawText(std::to_string(slots[slot_index].bits), rect, Justification::centred);
        }
      } else {
        g.setColour(Colours::black);
        g.drawRect(rect);
      }
    }
  }
}

void ValidatorGrid::update() {
  min_leading_bits = 257;
  max_leading_bits = 0;

  for (uint32_t i = 0; i < slots_number; ++i) {
    std::string diff = model->get_slot_difficulty(i);

    if (diff != slots[i].difficulty) {
      slots[i].owner = model->get_slot_owner(i);
      slots[i].difficulty = diff;  // BUG(kari): It is possible the difficulty to change while fetching the owner...
      slots[i].is_mine = slots[i].owner == owner ? true : false;
      slots[i].bits = leading_bits(slots[i].difficulty);
    }

    if (slots[i].bits && slots[i].bits < min_leading_bits) {
      min_leading_bits = slots[i].bits;
    }
    if (slots[i].bits > max_leading_bits) {
      max_leading_bits = slots[i].bits;
    }
  }
}

void ValidatorGrid::resized() {
  uint32_t w = getWidth();
  uint32_t h = getHeight();
  component_side_size_px = w < h ? w : h;

  slot_size = std::floor((static_cast<double>(component_side_size_px - 2) / slots_per_side));
  gap = slot_size > 3 ? 1 : 0;  // TODO(kari): Better formula
  gap = slot_size > 30 ? 2 : gap;
  slot_size -= gap;
  component_side_size_px = slots_per_side * (slot_size + gap) + gap;
}

uint32_t ValidatorGrid::leading_bits(const std::string& s) {
  uint32_t result = 0;
  for (uint32_t i = 0; i < s.size(); ++i) {
    uint8_t x = s[i];
    if (x == 0xFF) {
      result += 8;
    } else {
      while (x & 0x80) {
        result++;
        x <<= 1;
      }
      break;
    }
  }
  return result;
}

void ValidatorGrid::set_owner(const std::string& new_owner) {
  owner = new_owner;
}

void ValidatorGrid::allow_text_over_slots(bool allow) {
  allowed_text_over_slots = allow;
}
