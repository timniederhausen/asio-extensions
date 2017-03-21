/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <asioext/scoped_file_handle.hpp>
#include <asioext/read_file.hpp>
#include <asioext/write_file.hpp>
#include <asioext/open.hpp>

#include <asio/read.hpp>

#include <iostream>
#include <cassert>

int main(int argc, const char* argv[])
{
  const std::string test_content = "Hello world";

  try {
    // Utility functions write/read containers and buffer sequences to/from files.
    const std::array<asio::const_buffer, 2> buffers_to_write = {
      asio::buffer(test_content),
      asio::buffer(test_content),
    };
    asioext::write_file("myfile.txt", buffers_to_write);

    std::string read_content;
    asioext::read_file("myfile.txt", read_content);

    assert(read_content == test_content + test_content);

    // (scoped_)file_handle provides low-level access to files.
    // (There's also basic_file, which needs an asio::io_service and provides
    // asynchronous I/O.)
    asioext::scoped_file_handle file =
        asioext::open("myfile.txt", asioext::open_flags::access_read |
                                    asioext::open_flags::open_existing);

    assert(file.size() == test_content.size() * 2);

    std::string read_content2(test_content.size(), '\0');
    asio::read(file, asio::buffer(&read_content2[0], read_content2.size()));

    assert(read_content2 == test_content);
    return 0;
  } catch (std::exception& e) {
    // Exceptions are used for error reporting here.
    // All functions also offer a non-throwing overload,
    // which takes an asio::error_code& instead.
    std::cerr << "error: " << e.what() << std::endl;
    return 1;
  }
}
