/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <asioext/scoped_file_handle.hpp>
#include <asioext/open_flags.hpp>

#include <asio/write.hpp>

#include <iostream>

int main(int argc, const char* argv[])
{
  try {
    asioext::scoped_file_handle file("myfile.txt",
                                     asioext::access_write |
                                     asioext::create_always);

    const std::string content = "Hello world";
    asio::write(file, asio::buffer(content));
    return 0;
  } catch (std::exception& e) {
    // Exceptions are used for error reporting here.
    // All functions also offer a non-throwing overload,
    // which takes an asio::error_code instead.
    std::cerr << "error: " << e.what() << std::endl;
    return 1;
  }
}
