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

#include "Config.h"

#include "../../JuceLibraryCode/BinaryData.h"

using automaton::core::common::status;

Config::Config() {
  status s = load();
  if (s.code != automaton::core::common::status::OK) {
    throw std::runtime_error("Configuration error! " + s.msg);
  }
}

Config::~Config() {
  auto s = save_to_local_file();
  if (s.code != status::OK) {
    std::cout << "Failed to save local config file! " << s.msg << std::endl;
  }
  clearSingletonInstance();
}

status Config::load() {
  const ScopedLock Lock(critical_section);
  int file_size;
  const char* abi = BinaryData::getNamedResource("king_automaton_abi_json", file_size);
  if (abi == nullptr || file_size == 0) {
    return status::unavailable("Contract ABI not found!");
  } else {
    contract_abi = std::string(abi, file_size);
  }

  std::string file_content;
  File f = get_local_config_file();
  if (!f.exists()) {
    abi = BinaryData::getNamedResource("default_config_json", file_size);
    if (abi == nullptr || file_size == 0) {
      return status::unavailable("Config file not found!");
    } else {
      file_content = std::string(abi, file_size);
    }
  } else {
    file_content = f.loadFileAsString().toStdString();
  }
  try {
    std::stringstream ss(file_content);
    ss >> json_obj;
  } catch (const std::exception& e) {
    return status::unknown(e.what());
  }
  if (json_obj.is_null()) {
    return status::unknown("Json object is null!");
  }
  return status::ok();
}

void Config::set_json(const std::string& field, const std::string& data) {
  const ScopedLock Lock(critical_section);
  try {
    json_obj[field] = json::parse(data);
  } catch (const std::exception& e) {
    throw status::unknown(e.what());
  }
}

void Config::set_string(const std::string& field, const std::string& data) {
  const ScopedLock Lock(critical_section);
  json_obj[field] = data;
}

void Config::set_bool(const std::string& field, bool data) {
  const ScopedLock Lock(critical_section);
  json_obj[field] = data;
}

void Config::set_number(const std::string& field, int64_t data) {
  const ScopedLock Lock(critical_section);
  json_obj[field] = data;
}

status Config::save_to_local_file() {
  const ScopedLock Lock(critical_section);
  std::string new_file_content = json_obj.dump(2);
  File f = get_local_config_file();
  if (f.create().failed()) {
    std::cout << "File creation failed!" << std::endl;
    status s = status::unknown("File creation failed!");
    return s;
  }
  if (!f.replaceWithData(new_file_content.c_str(), new_file_content.size())) {
    status s = status::unknown("File was not saved!");
    return s;
  }
  return status::ok();
}

std::string Config::get_json(const std::string& field, const std::string& default_value) const {
  const ScopedLock Lock(critical_section);
  if (json_obj.find(field) != json_obj.end() && !json_obj[field].is_null()) {
    return json_obj[field].dump();
  }
  return default_value;
}

bool Config::get_bool(const std::string& field, bool default_value) const {
  const ScopedLock Lock(critical_section);
  if (json_obj.find(field) != json_obj.end() && json_obj[field].is_boolean()) {
    return json_obj[field].get<bool>();
  }
  return default_value;
}

int64_t Config::get_number(const std::string& field, int64_t default_value) const {
  const ScopedLock Lock(critical_section);
  if (json_obj.find(field) != json_obj.end() && json_obj[field].is_number()) {
    return json_obj[field].get<int64_t>();
  }
  return default_value;
}

bool Config::hasField(const std::string& field) const {
  const ScopedLock Lock(critical_section);
  return json_obj.find(field) != json_obj.end();
}

std::string Config::get_string(const std::string& field, const std::string& default_value) const {
  const ScopedLock Lock(critical_section);
  if (json_obj.find(field) != json_obj.end() && json_obj[field].is_string()) {
    return json_obj[field].get<std::string>();
  }
  return default_value;
}

std::string Config::get_abi() const {
  return contract_abi;
}

File Config::get_local_config_file() {
  return File::getSpecialLocation(File::userApplicationDataDirectory)
                .getChildFile("automaton")
                .getChildFile("config.json");
}

void Config::lock() {
  critical_section.enter();
}

void Config::unlock() {
  critical_section.exit();
}

JUCE_IMPLEMENT_SINGLETON(Config)
