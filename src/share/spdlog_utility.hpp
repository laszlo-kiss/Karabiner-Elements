#pragma once

#include "boost_defs.hpp"

#include <boost/lexical_cast.hpp>
#include <deque>
#include <iomanip>
#include <optional>
#include <spdlog/common.h>

namespace krbn {
class spdlog_utility final {
public:
  static std::string get_pattern(void) {
    return "[%Y-%m-%d %H:%M:%S.%e] [%l] [%n] %v";
  }

  static std::optional<uint64_t> get_sort_key(const std::string& line) {
    // line == "[2016-09-22 20:18:37.649] [info] [grabber] version 0.90.36"
    // return 20160922201837649

    // We can parse time strictly by using boost::posix_time::time_from_string.
    // But we cannot use it with boost header only.
    // So we use this rough way.

    if (line.size() < strlen("[0000-00-00 00:00:00.000]")) {
      return std::nullopt;
    }

    if (line.empty()) {
      return std::nullopt;
    }

    if (line[0] != '[') {
      return std::nullopt;
    }

    std::string result_string(4 + 2 + 2 +     // years,months,days
                                  2 + 2 + 2 + // hours,minutes,seconds
                                  3,          // milliseconds
                              '0');
    size_t line_pos = 1;
    size_t result_pos = 0;

    // years
    result_string[result_pos++] = line[line_pos++];
    result_string[result_pos++] = line[line_pos++];
    result_string[result_pos++] = line[line_pos++];
    result_string[result_pos++] = line[line_pos++];

    // months
    ++line_pos;
    result_string[result_pos++] = line[line_pos++];
    result_string[result_pos++] = line[line_pos++];

    // days
    ++line_pos;
    result_string[result_pos++] = line[line_pos++];
    result_string[result_pos++] = line[line_pos++];

    // hours
    ++line_pos;
    result_string[result_pos++] = line[line_pos++];
    result_string[result_pos++] = line[line_pos++];

    // minutes
    ++line_pos;
    result_string[result_pos++] = line[line_pos++];
    result_string[result_pos++] = line[line_pos++];

    // seconds
    ++line_pos;
    result_string[result_pos++] = line[line_pos++];
    result_string[result_pos++] = line[line_pos++];

    // milliseconds
    ++line_pos;
    result_string[result_pos++] = line[line_pos++];
    result_string[result_pos++] = line[line_pos++];
    result_string[result_pos++] = line[line_pos++];

    try {
      return boost::lexical_cast<uint64_t>(result_string);
    } catch (...) {
    }
    return std::nullopt;
  }

  static std::optional<spdlog::level::level_enum> get_level(const std::string& line) {
    auto front = strlen("[0000-00-00 00:00:00.000] [");
    if (line.size() <= front) {
      return std::nullopt;
    }

    for (int i = 0; i < spdlog::level::off; ++i) {
      auto level = spdlog::level::level_enum(i);
      auto level_name = std::string(spdlog::level::to_c_str(level)) + "]";

      if (line.compare(front, level_name.size(), level_name) == 0) {
        return level;
      }
    }

    return std::nullopt;
  }
};
} // namespace krbn
