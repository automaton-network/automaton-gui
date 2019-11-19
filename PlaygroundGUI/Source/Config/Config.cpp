#include "Config.h"

using automaton::core::common::status;

Config::Config() {
  status s = load();
  if (s.code != automaton::core::common::status::OK) {
    throw std::runtime_error("Configuration error! " + s.msg);
  }
}

Config::~Config() {
  clearSingletonInstance();
}

status Config::load() {
  status s = status::ok();
  const ScopedLock Lock(critical_section);
  File f =
      File::getSpecialLocation(File::userApplicationDataDirectory)
        .getChildFile("automaton")
        .getChildFile("config.json");
  if (!f.exists()) {
    f = File("../../Resources/default_config.json");
  }
  if (!f.exists()) {
    s.code = status::UNAVAILABLE;
    s.msg = "Configuration file not found!";
    return s;
  }
  std::string file_content = f.loadFileAsString().toStdString();
  try {
    std::stringstream ss(file_content);
    ss >> json_obj;
  } catch (const std::exception& e) {
    s.code = status::UNKNOWN;
    s.msg = e.what();
    return s;
  }
  if (json_obj.is_null()) {
    s.code = status::UNKNOWN;
    s.msg = "Json object is null!";
  }
  return s;
}

status Config::set_and_save(const std::string& field, const std::string& json_data) {
  const ScopedLock Lock(critical_section);
  std::stringstream ss;
  try {
    json j = json::parse(json_data);
    json_obj[field] = j;
    ss << json_obj;
  } catch (const std::exception& e) {
    status s = status::unknown(e.what());
    return s;
  }
  std::string new_file_content = ss.str();
  File f =
      File::getSpecialLocation(File::userApplicationDataDirectory)
        .getChildFile("automaton")
        .getChildFile("config.json");
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


JUCE_IMPLEMENT_SINGLETON(Config)
