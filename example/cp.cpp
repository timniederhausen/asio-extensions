/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <asioext/file_handle.hpp>
#include <asioext/open_flags.hpp>

#include <asio/write.hpp>
#include <asio/read.hpp>

#include <array>
#include <iostream>
#include <cstdio>

// While convenient, this method buffers the entire file which might not be
// possible/feasible for large files.
void copy_file_oneshot(asioext::file_handle& src, asioext::file_handle& dst)
{
  std::vector<char> buffer(static_cast<std::size_t>(src.size()));

  asio::error_code ec;
  const std::size_t actual = asio::read(src, asio::buffer(buffer), ec);

  if (ec && ec != asio::error::eof)
    return;

  asio::write(dst, asio::buffer(buffer.data(), actual));
}

// This function uses a stack-allocated buffer to copy data from |src| to |dst|
void copy_file_blocks(asioext::file_handle& src, asioext::file_handle& dst)
{
  std::array<char, 2048> buffer;

  for (bool at_end = false; !at_end; ) {
    // We need the actual bytes read here since the last block might be smaller.
    asio::error_code ec;
    const std::size_t actual = asio::read(src, asio::buffer(buffer), ec);

    if (ec) {
      if (ec == asio::error::eof)
        at_end = true;
      else
        throw asio::system_error(ec);
    }

    asio::write(dst, asio::buffer(buffer.data(), actual));
  }
}

void copy_file(const std::string& src_path, const std::string& dst_path)
{
  static const std::size_t kOneShotThreshold = 10 * 1024;

  // Exercise for the reader: Add support for '-' (i.e. stdout/stdin)
  asioext::file_handle src(src_path.c_str(),
                           asioext::open_flags::access_read |
                           asioext::open_flags::open_existing);

  asioext::file_handle dst(dst_path.c_str(),
                           asioext::open_flags::access_write |
                           asioext::open_flags::create_always);

  if (src.size() > kOneShotThreshold)
    copy_file_blocks(src, dst);
  else
    copy_file_oneshot(src, dst);
}

int main(int argc, const char* argv[])
{
  if (argc != 3) {
    std::cerr << "usage: cp source destination" << std::endl;
    return 1;
  }

  const std::string src_path = argv[1];
  const std::string dst_path = argv[2];

  try {
    copy_file(src_path, dst_path);
    return 0;
  } catch (std::exception& e) {
    std::cerr << "error: " << e.what() << std::endl;
    return 1;
  }
}
