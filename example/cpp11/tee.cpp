/// @copyright Copyright (c) 2017 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#include <asioext/scoped_file_handle.hpp>
#include <asioext/file_handle.hpp>
#include <asioext/open_flags.hpp>
#include <asioext/standard_streams.hpp>

#include <asio/write.hpp>

#include <iostream>
#include <cstdio>
#include <cassert>

using file_handles = std::vector<asioext::scoped_file_handle>;

bool tee_file(asioext::file_handle source, file_handles& destinations)
{
  char buffer[16 * 1024];
  std::error_code ec;

  while (true) {
    const std::size_t bytes_read = source.read_some(asio::buffer(buffer), ec);

    // Handle the expected errors here. (We could also use the throwing version
    // and catch & re-throw here, but this'd be overkill.)
    if (ec) {
      if (ec == asio::error::eof || ec == asio::error::broken_pipe)
        break;
      else
        throw std::system_error(ec);
    }

    for (std::size_t i = 0, n = destinations.size(); i != n; ++i)
      asio::write(destinations[i], asio::buffer(buffer, bytes_read));
  }

  return true;
}

int main(int argc, const char* argv[])
{
  if (argc < 2) {
    std::cerr << "usage: tee files..." << std::endl;
    return 1;
  }

  std::error_code ec;

  file_handles files(argc - 1);
  for (int i = 1; i != argc; ++i) {
    files[i - 1].open(argv[i], asioext::create_always | asioext::access_write,
                      ec);
    if (ec) {
      std::cerr << "Failed to open " << argv[i] << " with " << ec << '\n';
      return 1;
    }
  }

  try {
    return tee_file(asioext::get_stdin(), files) ? 0 : 1;
  } catch (std::exception& e) {
    std::cerr << "fatal: copying data failed with " << e.what() << '\n';
    return 1;
  }
}
