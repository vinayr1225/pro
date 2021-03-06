/*
   mkvmerge -- utility for splicing together matroska files
   from component media subtypes

   Distributed under the GPL v2
   see the file COPYING for details
   or visit http://www.gnu.org/copyleft/gpl.html

   Written by Moritz Bunkus <moritz@bunkus.org>.
*/

#pragma once

#include "common/common_pch.h"

#include "common/mm_io_p.h"

class mm_null_io_c;

class mm_null_io_private_c : public mm_io_private_c {
public:
  int64_t pos{};
  std::string file_name;

  explicit mm_null_io_private_c(std::string const &p_file_name)
    : file_name{p_file_name}
  {
  }
};
