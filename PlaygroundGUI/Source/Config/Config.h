/*
 * Automaton Playground
 * Copyright (C) 2019 Asen Kovachev (@asenski, GitHub: akovachev)
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

#include "../../JuceLibraryCode/JuceHeader.h"

#include <json.hpp>

#include "automaton/core/common/status.h"

using json = nlohmann::json;

class Config {
 public:
  Config();
  ~Config();

  automaton::core::common::status load();
  automaton::core::common::status set_and_save(const std::string& field, const std::string& json_data);
  std::string get_json(const std::string& field);
  bool get_bool(const std::string& field);
  int64_t get_number(const std::string& field);
  std::string get_string(const std::string& field);

  JUCE_DECLARE_SINGLETON(Config, false)
 private:
  std::string default_path = "../../Resources/default_config.json";

  json json_obj;

  CriticalSection critical_section;
};
