/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// This examples requires ASIOEXT_USE_BOOST_ASIO to be defined.
// Either set it on the command-line or #define it before #including any
// AsioExt headers.

#include <asioext/scoped_file_handle.hpp>
#include <asioext/file_handle.hpp>
#include <asioext/open.hpp>

#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/array.hpp>

#include <iostream>
#include <cstdio>

namespace asio = boost::asio;

// General-purpose stream copying from |src| to |dst|.
// This doesn't use src.size() deliberately, so we can support special
// files that don't report their size correctly.
void copy_file_aux(asioext::file_handle src, asioext::file_handle dst)
{
  static const std::size_t kBufferSize = 16 * 1024;

  boost::array<char, kBufferSize> buffer;
  while (true) {
    boost::system::error_code ec;
    const std::size_t actual = asio::read(src, asio::buffer(buffer), ec);

    // Handle the expected errors here. (We could also use the throwing version
    // and catch & re-throw here, but this'd be overkill.)
    if (ec) {
      if (ec == asio::error::eof)
        break;
      else
        throw boost::system::system_error(ec);
    }

    asio::write(dst, asio::buffer(buffer.data(), actual));
  }
}

bool copy_file(const boost::filesystem::path& src_path,
               const boost::filesystem::path& dst_path)
{
  // Exercise for the reader: Add support for '-' (i.e. stdout/stdin)
  asioext::scoped_file_handle src;

  try {
    src.reset(asioext::open(src_path,
                            asioext::open_flags::access_read |
                            asioext::open_flags::open_existing).release());
  } catch (std::exception& e) {
    std::cerr << "error: Failed to open " << src_path << " with: "
        << e.what() << '\n';
    return false;
  }

  asioext::scoped_file_handle dst;

  try {
    dst.reset(asioext::open(dst_path,
                            asioext::open_flags::access_write |
                            asioext::open_flags::create_always).release());
  } catch (std::exception& e) {
    std::cerr << "error: Failed to open " << dst_path << " with: "
        << e.what() << '\n';
    return false;
  }

  try {
    copy_file_aux(src.get(), dst.get());
  } catch (std::exception& e) {
    std::cerr << "error: Copying data failed with " << e.what() << '\n';
    return false;
  }

  return true;
}

int main(int argc, const char* argv[])
{
  if (argc != 3) {
    std::cerr << "usage: cp source destination" << std::endl;
    return 1;
  }

  const boost::filesystem::path src_path(argv[1]);
  const boost::filesystem::path dst_path(argv[2]);

  std::cerr << "cp " << src_path << " to " << dst_path << '\n';
  return copy_file(src_path, dst_path) ? 0 : 1;
}
