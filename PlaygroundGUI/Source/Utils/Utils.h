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

enum class CoinUnit {Gwei = 9, ether = 18, AUTO = 18};

class Utils {
 public:
  static std::string gen_ethereum_address(const std::string& privkey_hex);
  static std::unique_ptr<Drawable> loadSVG(const String& xmlData);
  static String fromWei(CoinUnit unitTo, const String& value);
  static String toWei(CoinUnit unitTo, const String& value);

  static const String numericalIntegerAllowed;
  static const String numericalFloatAllowed;
};
