/*
   mkvmerge -- utility for splicing together matroska files
   from component media subtypes

   Distributed under the GPL v2
   see the file COPYING for details
   or visit http://www.gnu.org/copyleft/gpl.html

   string helper functions

   Written by Moritz Bunkus <moritz@bunkus.org>.
*/

#pragma once

#include "common/common_pch.h"

enum class line_ending_style_e {
  cr_lf,
  lf,
};

std::string normalize_line_endings(std::string const &str, line_ending_style_e line_ending_style = line_ending_style_e::lf);
std::string chomp(std::string const &str);

std::vector<std::string> split(std::string const &text, boost::regex const &pattern, size_t max = 0, boost::match_flag_type match_flags = boost::match_default);

inline std::vector<std::string>
split(std::string const &text,
      std::string const &pattern = ",",
      size_t max = 0) {
  return split(text, boost::regex("\\Q"s + pattern, boost::regex::perl), max);
}

void strip(std::string &s, bool newlines = false);
std::string strip_copy(std::string const &s, bool newlines = false);
void strip(std::vector<std::string> &v, bool newlines = false);
void strip_back(std::string &s, bool newlines = false);

std::string &shrink_whitespace(std::string &s);

std::string get_displayable_string(const char *src, int max_len = -1);
std::string get_displayable_string(std::string const &src);

extern const std::string empty_string;
