/*
   mkvmerge -- utility for splicing together matroska files
   from component media subtypes

   Distributed under the GPL v2
   see the file COPYING for details
   or visit http://www.gnu.org/copyleft/gpl.html

   Video for WIndows output module

   Written by Moritz Bunkus <moritz@bunkus.org>.
*/

#include "common/common_pch.h"

#include <matroska/KaxTracks.h>

#include "avilib.h"
#include "common/codec.h"
#include "common/debugging.h"
#include "common/endian.h"
#include "common/hacks.h"
#include "common/mpeg4_p2.h"
#include "merge/connection_checks.h"
#include "output/p_video_for_windows.h"

using namespace libmatroska;

video_for_windows_packetizer_c::video_for_windows_packetizer_c(generic_reader_c *p_reader,
                                                               track_info_c &p_ti,
                                                               double fps,
                                                               int width,
                                                               int height)
  : generic_video_packetizer_c{p_reader, p_ti, MKV_V_MSCOMP, fps, width, height}
  , m_rederive_frame_types{debugging_c::requested("rederive_frame_types")}
  , m_codec_type{ct_unknown}
{
}

void
video_for_windows_packetizer_c::check_fourcc() {
  if (   (m_hcodec_id                != MKV_V_MSCOMP)
      || !m_ti.m_private_data
      || (sizeof(alBITMAPINFOHEADER) >  m_ti.m_private_data->get_size()))
    return;

  if (!m_ti.m_fourcc.empty()) {
    memcpy(&reinterpret_cast<alBITMAPINFOHEADER *>(m_ti.m_private_data->get_buffer())->bi_compression, m_ti.m_fourcc.c_str(), 4);
    set_codec_private(m_ti.m_private_data);
  }

  char fourcc[5];
  memcpy(fourcc, &reinterpret_cast<alBITMAPINFOHEADER *>(m_ti.m_private_data->get_buffer())->bi_compression, 4);
  fourcc[4] = 0;

  if (mpeg4::p2::is_v3_fourcc(fourcc))
    m_codec_type = video_for_windows_packetizer_c::ct_div3;

  else if (mpeg4::p2::is_fourcc(fourcc))
    m_codec_type = video_for_windows_packetizer_c::ct_mpeg4_p2;
}

void
video_for_windows_packetizer_c::set_headers() {
  generic_video_packetizer_c::set_headers();
  check_fourcc();
}

int
video_for_windows_packetizer_c::process(packet_cptr packet) {
  if (m_rederive_frame_types)
    rederive_frame_type(packet);

  return generic_video_packetizer_c::process(packet);
}

void
video_for_windows_packetizer_c::rederive_frame_type(packet_cptr &packet) {
  switch (m_codec_type) {
    case video_for_windows_packetizer_c::ct_div3:
      rederive_frame_type_div3(packet);
      break;

    case video_for_windows_packetizer_c::ct_mpeg4_p2:
      rederive_frame_type_mpeg4_p2(packet);
      break;

    default:
      break;
  }
}

void
video_for_windows_packetizer_c::rederive_frame_type_div3(packet_cptr &packet) {
  if (1 >= packet->data->get_size())
    return;

  packet->bref = packet->data->get_buffer()[0] & 0x40 ? VFT_PFRAMEAUTOMATIC : VFT_IFRAME;
  packet->fref = VFT_NOBFRAME;
}

void
video_for_windows_packetizer_c::rederive_frame_type_mpeg4_p2(packet_cptr &packet) {
  size_t idx, size    = packet->data->get_size();
  unsigned char *data = packet->data->get_buffer();

  for (idx = 0; idx < size - 5; ++idx) {
    if ((0x00 == data[idx]) && (0x00 == data[idx + 1]) && (0x01 == data[idx + 2])) {
      if ((0 == data[idx + 3]) || (0xb0 == data[idx + 3])) {
        packet->bref = VFT_IFRAME;
        packet->fref = VFT_NOBFRAME;
        return;
      }

      if (0xb6 == data[idx + 3]) {
        packet->bref = 0x00 == (data[idx + 4] & 0xc0) ? VFT_IFRAME : VFT_PFRAMEAUTOMATIC;
        packet->fref = VFT_NOBFRAME;
        return;
      }

      idx += 2;
    }
  }
}
