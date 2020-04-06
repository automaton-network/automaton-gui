/*
 * Automaton Playground
 * Copyright (c) 2019 The Automaton Playground Authors.
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

#include "../../JuceLibraryCode/JuceHeader.h"

#include <json.hpp>

#include "automaton/core/common/status.h"

using json = nlohmann::json;

class Config {
 public:
  Config();
  Config(const Config& other);
  Config& operator=(const Config& other);
  virtual ~Config();

  void set_json(const std::string& field, const json& data);
  void set_string(const std::string& field, const std::string& data);
  void set_bool(const std::string& field, bool data);
  void set_number(const std::string& field, int64_t data);

  json get_json(const std::string& field, const json& default_value = json()) const;
  std::string get_string(const std::string& field, const std::string& default_value = std::string()) const;
  bool get_bool(const std::string& field, bool default_value = false) const;
  int64_t get_number(const std::string& field, int64_t default_value = 0) const;
  bool hasField(const std::string& field) const;

  const json& to_json();
  void restoreFrom_json(const json& data);

 protected:
  virtual void config_changed();
  json json_obj;
  CriticalSection critical_section;
};

class ConfigFile : public Config
                 , public DeletedAtShutdown {
 public:
  ConfigFile();
  ~ConfigFile();

  automaton::core::common::status load();
  automaton::core::common::status save_to_local_file();

  JUCE_DECLARE_SINGLETON(ConfigFile, false)

 private:
  File get_local_config_file();
};
