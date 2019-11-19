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
  save_to_local_file();
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

status Config::set(const std::string& field, const std::string& json_data) {
  const ScopedLock Lock(critical_section);
  try {
    json j = json::parse(json_data);
    json_obj[field] = j;
  } catch (const std::exception& e) {
    status s = status::unknown(e.what());
    return s;
  }
  return status::ok();
}

status Config::save_to_local_file() {
  const ScopedLock Lock(critical_section);
  std::stringstream ss;
  ss << json_obj;
  std::string new_file_content = ss.str();
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

std::string Config::get_json(const std::string& field) {
  std::string res = "";
  if (json_obj.find(field) != json_obj.end() && !json_obj[field].is_null()) {
    res = json_obj[field].dump();
  }
  return res;
}

bool Config::get_bool(const std::string& field) {
  bool res = false;
  if (json_obj.find(field) != json_obj.end() && json_obj[field].is_boolean()) {
    // TODO(kari): handle errors
    res = json_obj[field].get<bool>();
  }
  return res;
}

int64_t Config::get_number(const std::string& field) {
  int64_t res = 0;
  if (json_obj.find(field) != json_obj.end() && json_obj[field].is_number()) {
    res = json_obj[field].get<int64_t>();
  }
  return res;
}

std::string Config::get_string(const std::string& field) {
  std::string res = "";
  if (json_obj.find(field) != json_obj.end() && json_obj[field].is_string()) {
    res = json_obj[field].get<std::string>();
  }
  return res;
}

std::string Config::get_abi() {
  return contract_abi;
}

File Config::get_local_config_file() {
  return File::getSpecialLocation(File::userApplicationDataDirectory)
                .getChildFile("automaton")
                .getChildFile("config.json");
}


JUCE_IMPLEMENT_SINGLETON(Config)
