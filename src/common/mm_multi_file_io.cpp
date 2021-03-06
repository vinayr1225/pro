/*
   mkvmerge -- utility for splicing together matroska files
   from component media subtypes

   Distributed under the GPL v2
   see the file COPYING for details
   or visit http://www.gnu.org/copyleft/gpl.html

   IO callback class definitions

   Written by Moritz Bunkus <moritz@bunkus.org>.
*/

#include "common/common_pch.h"

#include <sstream>

#include "common/id_info.h"
#include "common/mm_io_x.h"
#include "common/mm_file_io.h"
#include "common/mm_multi_file_io.h"
#include "common/mm_multi_file_io_p.h"
#include "common/output.h"
#include "common/strings/editing.h"
#include "common/strings/parsing.h"

mm_multi_file_io_c::mm_multi_file_io_c(std::vector<bfs::path> const &file_names,
                                       std::string const &display_file_name)
  : mm_io_c{*new mm_multi_file_io_private_c{file_names, display_file_name}}
{
}

mm_multi_file_io_c::mm_multi_file_io_c(mm_multi_file_io_private_c &p)
  : mm_io_c{p}
{
}

mm_multi_file_io_c::~mm_multi_file_io_c() {
  close_multi_file_io();
}

uint64
mm_multi_file_io_c::getFilePointer() {
  return p_func()->current_pos;
}

void
mm_multi_file_io_c::setFilePointer(int64 offset,
                                   libebml::seek_mode mode) {
  auto p = p_func();

  int64_t new_pos
    = libebml::seek_beginning == mode ? offset
    : libebml::seek_end       == mode ? p->total_size  + offset // offsets from the end are negative already
    :                                   p->current_pos + offset;

  if ((0 > new_pos) || (static_cast<int64_t>(p->total_size) < new_pos))
    throw mtx::mm_io::seek_x();

  p->current_file = 0;
  for (auto &file : p->files) {
    if ((file.global_start + file.size) < static_cast<uint64_t>(new_pos)) {
      ++p->current_file;
      continue;
    }

    p->current_pos       = new_pos;
    p->current_local_pos = new_pos - file.global_start;
    file.file->setFilePointer(p->current_local_pos);
    break;
  }
}

uint32
mm_multi_file_io_c::_read(void *buffer,
                          size_t size) {
  auto p                = p_func();

  size_t num_read_total = 0;
  auto buffer_ptr       = static_cast<unsigned char *>(buffer);

  while (!eof() && (num_read_total < size)) {
    auto &file       = p->files[p->current_file];
    auto num_to_read = static_cast<uint64_t>(std::min(static_cast<uint64_t>(size) - static_cast<uint64_t>(num_read_total), file.size - p->current_local_pos));

    if (0 != num_to_read) {
      size_t num_read       = file.file->read(buffer_ptr, num_to_read);
      num_read_total       += num_read;
      buffer_ptr           += num_read;
      p->current_local_pos += num_read;
      p->current_pos       += num_read;

      if (num_read != num_to_read)
        break;
    }

    if ((p->current_local_pos >= file.size) && (p->files.size() > (p->current_file + 1))) {
      ++p->current_file;
      p->current_local_pos = 0;
      p->files[p->current_file].file->setFilePointer(0);
    }
  }

  return num_read_total;
}

size_t
mm_multi_file_io_c::_write(const void *,
                           size_t) {
  throw mtx::mm_io::wrong_read_write_access_x();
}

void
mm_multi_file_io_c::close() {
  close_multi_file_io();
}

void
mm_multi_file_io_c::close_multi_file_io() {
  auto p = p_func();

  for (auto &file : p->files)
    file.file->close();

  p->files.clear();
  p->total_size        = 0;
  p->current_pos       = 0;
  p->current_local_pos = 0;
}

bool
mm_multi_file_io_c::eof() {
  auto p = p_func();

  return p->files.empty() || ((p->current_file == (p->files.size() - 1)) && (p->current_local_pos >= p->files[p->current_file].size));
}

std::vector<bfs::path>
mm_multi_file_io_c::get_file_names() {
  auto p = p_func();

  std::vector<bfs::path> file_names;
  for (auto &file : p->files)
    file_names.push_back(file.file_name);

  return file_names;
}

void
mm_multi_file_io_c::create_verbose_identification_info(mtx::id::info_c &info) {
  auto p          = p_func();
  auto file_names = nlohmann::json::array();
  for (auto &file : p->files)
    if (file.file_name != p->files.front().file_name)
    file_names.push_back(file.file_name.string());

  info.add(mtx::id::other_file, file_names);
}

void
mm_multi_file_io_c::display_other_file_info() {
  auto p = p_func();

  std::stringstream out;

  for (auto &file : p->files)
    if (file.file_name != p->files.front().file_name) {
      if (!out.str().empty())
        out << ", ";
      out << file.file_name.filename();
    }

  if (!out.str().empty())
    mxinfo(fmt::format(Y("'{0}': Processing the following files as well: {1}\n"), p->display_file_name, out.str()));
}

void
mm_multi_file_io_c::enable_buffering(bool enable) {
  auto p = p_func();

  for (auto &file : p->files)
    file.file->enable_buffering(enable);
}

std::string
mm_multi_file_io_c::get_file_name()
  const {
  return p_func()->display_file_name;
}

mm_io_cptr
mm_multi_file_io_c::open_multi(const std::string &display_file_name,
                               bool single_only) {
  bfs::path first_file_name(bfs::system_complete(bfs::path(display_file_name)));
  std::string base_name = bfs::basename(first_file_name);
  std::string extension = balg::to_lower_copy(bfs::extension(first_file_name));
  boost::regex file_name_re("(.+[_\\-])(\\d+)$", boost::regex::perl);
  boost::smatch matches;

  if (!boost::regex_match(base_name, matches, file_name_re) || single_only) {
    std::vector<bfs::path> file_names;
    file_names.push_back(first_file_name);
    return mm_io_cptr(new mm_multi_file_io_c(file_names, display_file_name));
  }

  int start_number = 1;
  parse_number(matches[2].str(), start_number);

  base_name = balg::to_lower_copy(matches[1].str());

  std::vector<std::pair<int, bfs::path>> paths;
  paths.emplace_back(start_number, first_file_name);

  bfs::directory_iterator end_itr;
  for (bfs::directory_iterator itr(first_file_name.branch_path()); itr != end_itr; ++itr) {
    if (   bfs::is_directory(itr->status())
        || !balg::iequals(bfs::extension(itr->path()), extension))
      continue;

    std::string stem   = bfs::basename(itr->path());
    int current_number = 0;

    if (   !boost::regex_match(stem, matches, file_name_re)
        || !balg::iequals(matches[1].str(), base_name)
        || !parse_number(matches[2].str(), current_number)
        || (current_number <= start_number))
      continue;

    paths.emplace_back(current_number, itr->path());
  }

  brng::sort(paths);

  std::vector<bfs::path> file_names;
  for (auto &path : paths)
    file_names.emplace_back(std::get<1>(path));

  return mm_io_cptr(new mm_multi_file_io_c(file_names, display_file_name));
}
