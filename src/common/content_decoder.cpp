/*
   mkvmerge -- utility for splicing together matroska files
   from component media subtypes

   Distributed under the GPL v2
   see the file COPYING for details
   or visit http://www.gnu.org/copyleft/gpl.html

   Helper routines for various compression libs

   Written by Moritz Bunkus <moritz@bunkus.org>.
*/

#include "common/common_pch.h"

#include "common/content_decoder.h"
#include "common/ebml.h"
#include "common/list_utils.h"
#include "common/strings/formatting.h"

using namespace libmatroska;

kax_content_encoding_t::kax_content_encoding_t()
  : order{}
  , type{}
  , scope(CONTENT_ENCODING_SCOPE_BLOCK)
  , comp_algo{}
  , enc_algo{}
  , sig_algo{}
  , sig_hash_algo{}
{
}

content_decoder_c::content_decoder_c()
  : ok{true}
{
}

content_decoder_c::content_decoder_c(KaxTrackEntry &ktentry)
  : ok{true}
{
  initialize(ktentry);
}

bool
content_decoder_c::initialize(KaxTrackEntry &ktentry) {
  encodings.clear();

  auto kcencodings = FindChild<KaxContentEncodings>(&ktentry);
  if (!kcencodings)
    return true;

  int tid = kt_get_number(ktentry);

  for (auto kcenc_el : *kcencodings) {
    auto kcenc = dynamic_cast<KaxContentEncoding *>(kcenc_el);
    if (!kcenc)
      continue;

    kax_content_encoding_t enc;

    enc.order    = FindChildValue<KaxContentEncodingOrder>(kcenc);
    enc.type     = FindChildValue<KaxContentEncodingType >(kcenc);
    enc.scope    = FindChildValue<KaxContentEncodingScope>(kcenc, 1u);

    auto ce_comp = FindChild<KaxContentCompression>(kcenc);
    if (ce_comp) {
      enc.comp_algo     = FindChildValue<KaxContentCompAlgo    >(ce_comp);
      enc.comp_settings = FindChildValue<KaxContentCompSettings>(ce_comp);
    }

    auto ce_enc = FindChild<KaxContentEncryption>(kcenc);
    if (ce_enc) {
      enc.enc_algo      = FindChildValue<KaxContentEncAlgo    >(ce_enc);
      enc.enc_keyid     = FindChildValue<KaxContentEncKeyID   >(ce_enc);
      enc.sig_algo      = FindChildValue<KaxContentSigAlgo    >(ce_enc);
      enc.sig_hash_algo = FindChildValue<KaxContentSigHashAlgo>(ce_enc);
      enc.sig_keyid     = FindChildValue<KaxContentSigKeyID   >(ce_enc);
      enc.signature     = FindChildValue<KaxContentSignature  >(ce_enc);
    }

    if (1 == enc.type) {
      mxwarn(fmt::format(Y("Track number {0} has been encrypted and decryption has not yet been implemented.\n"), tid));
      ok = false;
      break;
    }

    if (0 != enc.type) {
      mxerror(fmt::format(Y("Unknown content encoding type {0} for track {1}.\n"), enc.type, tid));
      ok = false;
      break;
    }

    if (0 == enc.comp_algo)
      enc.compressor = std::shared_ptr<compressor_c>(new zlib_compressor_c());

    else if (mtx::included_in(enc.comp_algo, 1u, 2u)) {
      auto algorithm = 1u == enc.comp_algo ? "bzlib" : "lzo1x";
      mxwarn(fmt::format(Y("Track {0} was compressed with the algorithm '{1}' which is not supported anymore.\n"), tid, algorithm));
      ok = false;
      break;

    } else if (3 == enc.comp_algo) {
      enc.compressor = std::shared_ptr<compressor_c>(new header_removal_compressor_c);
      std::static_pointer_cast<header_removal_compressor_c>(enc.compressor)->set_bytes(enc.comp_settings);

    } else {
      mxwarn(fmt::format(Y("Track {0} has been compressed with an unknown/unsupported compression algorithm ({1}).\n"), tid, enc.comp_algo));
      ok = false;
      break;
    }

    encodings.push_back(enc);
  }

  brng::stable_sort(encodings, [](kax_content_encoding_t const &a, kax_content_encoding_t const &b) { return a.order < b.order; });

  return ok;
}

void
content_decoder_c::reverse(memory_cptr &memory,
                           content_encoding_scope_e scope) {
  if (!is_ok() || encodings.empty())
    return;

  for (auto &ce : encodings)
    if (0 != (ce.scope & scope))
      memory = ce.compressor->decompress(memory);
}

std::string
content_decoder_c::descriptive_algorithm_list() {
  std::string list;
  std::vector<std::string> algorithms;

  boost::for_each(encodings, [&algorithms](auto const &enc) { algorithms.push_back(to_string(enc.comp_algo)); });

  return boost::join(algorithms, ",");
}
