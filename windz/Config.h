//
// Created by crystalwind on 19-1-18.
//

#ifndef WINDZ_CONFIG_H
#define WINDZ_CONFIG_H

#include <string>
#include <map>
#include <list>

namespace windz {

class Config {
  public:
    //  0 success
    // -1 IO error
    // >0 error line's line num
    int Parse(const std::string &filename);

    // Get string from config file, return default_value if not found.
    std::string GetString(std::string section, std::string name, std::string default_value);

    // Get integer from config file, return default_value if not found
    // or not a valid integer according to strtol().
    long GetInt(std::string section, std::string name, long default_value);

    // Get double or float from config file, return default_value if not found
    // or not a valid double or float according to strtod().
    double GetDouble(std::string section, std::string name, double default_value);

    // Get bool from config file, return default_value if not found or not a valid bool.
    // Valid bool(case insensitive): true/false, yes/no, on/off, 1/0.
    bool GetBool(std::string section, std::string name, bool default_value);

    // Get list from config file, return empty list if not found.
    std::list<std::string> GetList(std::string section, std::string name);

    // For test
    std::map<std::string, std::list<std::string>> &config() { return config_; };

  private:
    std::string filename_;
    std::map<std::string, std::list<std::string>> config_;
};

}  // namespace windz

#endif //WINDZ_CONFIG_H
