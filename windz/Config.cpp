//
// Created by crystalwind on 19-1-18.
//

#include "Config.h"
#include <stdlib.h>
#include <ctype.h>
#include <string>
#include <map>
#include <list>
#include <memory>
#include <algorithm>

using namespace std;

namespace windz {

namespace {

string MakeKey(const string &section, const string &name) {
    string key = section + "." + name;
    transform(key.begin(), key.end(), key.begin(), ::tolower);
    return key;
}

class LineParser {
  public:
    explicit LineParser(const char *line) : line_(line) {}

    void Assign(const char *line) {
        line_ = line;
    }

    LineParser &SkipSpace() {
        while (isspace(*line_)) {
            ++line_;
        }
        return *this;
    }
    string Strip(const char *begin, const char *end) {
        while (begin < end && isspace(*begin)) {
           ++begin;
        }
        while (end > begin && isspace(*(end-1))) {
            --end;
        }

        return string(begin, end);
    }
    char Peek() {
        SkipSpace();
        return *line_;
    }
    LineParser &Skip(int n) {
        line_ += n;
        return *this;
    }
    bool Match(char c) {
        SkipSpace();
        return (*line_ == c);
    }
    string Till(char c) {
        SkipSpace();
        const char *end = line_;
        while (*end && *end != c) {
            ++end;
        }
        if (*end != c) {
            return "";
        }
        const char *begin = line_;
        line_ = end;
        return Strip(begin, end);
    }
    string TillEnd() {
        SkipSpace();
        const char *end = line_;
        while (*end && !isspace(*end) && *end != '#') {
            ++end;
        }
        const char *begin = line_;
        line_ = end;
        return Strip(begin, end);
    }
    string TillSpace() {
        SkipSpace();
        const char *end = line_;
        while (*end && !isspace(*end)) {
            ++end;
        }
        const char *begin = line_;
        line_ = end;
        return Strip(begin, end);
    }

  private:
    const char *line_;
};

}  // namespace

int Config::Parse(const std::string &filename) {
    filename_ = filename;
    FILE *file = fopen(filename_.c_str(), "r");
    if (!file) {
        return -1;
    }
    unique_ptr<FILE, decltype(fclose) *> closefile(file, fclose);

    static const int MAXLINE = 1024;
    char line[MAXLINE];
    int line_num = 0;
    string section, name;
    LineParser line_parser(line);
    bool error = false;

    while (!error && fgets(line, MAXLINE, file) != nullptr) {
        ++line_num;
        line_parser.Assign(line);
        char c = line_parser.Peek();
        if (c == '#' || c == '\0') { // comment or blank line
            continue;
        } else if (c == '[') {  // section
            section = line_parser.Skip(1).Till(']');
            error = !line_parser.Match(']');
            name.clear();
        } else if (isspace(line[0])) {
            // Non-blank line with leading whitespace,
            // treat as continuation of previous name's value.
            if (!name.empty()) {
                config_[MakeKey(section, name)].push_back(line_parser.TillEnd());
            } else {
                error = true;
            }
        } else {
            LineParser backup = line_parser;
            name = line_parser.Till('=');
            if (line_parser.Match('=')) {
                /* empty */;
            } else {
                line_parser = backup;
                name = line_parser.Till(':');
                error = !line_parser.Match(':');
            }
            if (!error){
                config_[MakeKey(section, name)].push_back(line_parser.Skip(1).TillSpace());
            }
        }
    }

    return error ? line_num : 0;
}

string Config::GetString(string section, string name, string default_value) {
    string key = MakeKey(section, name);
    auto iter = config_.find(key);
    if (iter == config_.end()) {
        return default_value;
    } else {
        return iter->second.front();
    }
}

long Config::GetInt(string section, string name, long default_value) {
    string int_str = GetString(section, name, "");
    const char *begin = int_str.c_str();
    char *end;
    long val = strtol(begin, &end, 0);
    return end > begin ? val : default_value;
}

double Config::GetDouble(string section, std::string name, double default_value) {
    string double_str = GetString(section, name, "");
    const char *begin = double_str.c_str();
    char *end;
    double val = strtod(begin, &end);
    return end > begin ? val : default_value;
}

bool Config::GetBool(string section, std::string name, bool default_value) {
    string bool_str = GetString(section, name, "");
    transform(bool_str.begin(), bool_str.end(), bool_str.begin(), ::tolower);
    if (bool_str == "true" || bool_str == "yes" || bool_str == "on" || bool_str == "1") {
        return true;
    } else if (bool_str == "false" || bool_str == "no" || bool_str == "off" || bool_str == "0") {
        return false;
    } else{
        return default_value;
    }
}

list<std::string> Config::GetList(string section, string name) {
    string key = MakeKey(section, name);
    auto iter = config_.find(key);
    if (iter == config_.end()) {
        return list<string>();
    } else {
        return iter->second;
    }
}

}  // namespace windz