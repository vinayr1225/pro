/*
   mkvmerge -- utility for splicing together matroska files
   from component media subtypes

   Distributed under the GPL v2
   see the file COPYING for details
   or visit http://www.gnu.org/copyleft/gpl.html

   Subripper subtitle reader

   Written by Moritz Bunkus <moritz@bunkus.org>.
*/

#include "common/common_pch.h"

#include "common/codec.h"
#include "common/id_info.h"
#include "common/mm_proxy_io.h"
#include "common/mm_text_io.h"
#include "input/r_srt.h"
#include "input/subtitles.h"
#include "merge/input_x.h"

int
srt_reader_c::probe_file(mm_text_io_c &in,
                         uint64_t) {
  return srt_parser_c::probe(in);
}

srt_reader_c::srt_reader_c(const track_info_c &ti,
                           const mm_io_cptr &in)
  : generic_reader_c(ti, in)
{
}

void
srt_reader_c::read_headers() {
  try {
    m_text_in = std::make_shared<mm_text_io_c>(m_in);
    if (!srt_parser_c::probe(*m_text_in))
      throw mtx::input::invalid_format_x();

    m_ti.m_id = 0;                 // ID for this track.
    m_subs    = std::make_shared<srt_parser_c>(m_text_in, m_ti.m_fname, 0);

  } catch (...) {
    throw mtx::input::open_x();
  }

  show_demuxer_info();

  m_subs->parse();

  m_bytes_to_process = m_subs->get_total_byte_size();
}

srt_reader_c::~srt_reader_c() {
}

void
srt_reader_c::create_packetizer(int64_t) {
  if (!demuxing_requested('s', 0) || (NPTZR() != 0))
    return;

  auto need_recoding = m_text_in->get_byte_order() == BO_NONE;
  add_packetizer(new textsubs_packetizer_c(this, m_ti, MKV_S_TEXTUTF8, need_recoding));

  show_packetizer_info(0, PTZR0);
}

file_status_e
srt_reader_c::read(generic_packetizer_c *,
                   bool) {
  if (!m_subs->empty()) {
    m_bytes_processed += m_subs->get_next_byte_size();
    m_subs->process(PTZR0);
  }

  return m_subs->empty() ? flush_packetizers() : FILE_STATUS_MOREDATA;
}

int64_t
srt_reader_c::get_progress() {
  return m_bytes_processed;
}

int64_t
srt_reader_c::get_maximum_progress() {
  return m_bytes_to_process;
}

void
srt_reader_c::identify() {
  auto info     = mtx::id::info_c{};
  auto encoding = m_text_in->get_encoding();

  info.add(mtx::id::text_subtitles, true);
  if (encoding)
    info.add(mtx::id::encoding, *encoding);

  id_result_container();
  id_result_track(0, ID_RESULT_TRACK_SUBTITLES, codec_c::get_name(codec_c::type_e::S_SRT, "SRT"), info.get());
}
