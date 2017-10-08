/*
   mkvextract -- extract tracks from Matroska files into other files

   Distributed under the GPL v2
   see the file COPYING for details
   or visit http://www.gnu.org/copyleft/gpl.html

   Written by Moritz Bunkus <moritz@bunkus.org>.
*/

#pragma once

#include "common/common_pch.h"

#include <unordered_map>

#include "common/cli_parser.h"
#include "extract/mkvextract.h"
#include "extract/options.h"

class extract_cli_parser_c: public mtx::cli::parser_c {
protected:
  enum class cli_type_e {
    unknown,
    single,
    multiple,
  };

  cli_type_e m_cli_type;
  options_c m_options;
  std::vector<options_c::mode_options_c>::iterator m_current_mode;
  int m_num_unknown_args;

  std::string m_charset;
  bool m_extract_cuesheet;
  int m_extract_blockadd_level;
  track_spec_t::target_mode_e m_target_mode;

  std::unordered_map<options_c::extraction_mode_e, std::unordered_map<int64_t, bool>> m_used_tids;

  debugging_option_c m_debug;

public:
  extract_cli_parser_c(const std::vector<std::string> &args);

  options_c run();

protected:
  void init_parser();
  void set_default_values();

  void assert_mode(options_c::extraction_mode_e mode);

  void set_parse_fully();
  void set_charset();
  void set_cuesheet();
  void set_blockadd();
  void set_raw();
  void set_fullraw();
  void set_simple();
  void set_simple_language();
  void set_cli_mode();
  void set_extraction_mode();
  void add_extraction_spec();
  void handle_unknown_arg();
  void handle_unknown_arg_single_mode();
  void handle_unknown_arg_multiple_mode();
  void verify_options_multiple_mode();

protected:
  static boost::optional<options_c::extraction_mode_e> extraction_mode_from_string(std::string const &mode_string);
  static std::string extraction_mode_to_string(boost::optional<options_c::extraction_mode_e> mode);
};
