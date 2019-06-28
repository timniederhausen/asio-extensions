/// Copyright (c) 2016 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#include "test_file_writer.hpp"

#include "asioext/unique_file_handle.hpp"
#include "asioext/open.hpp"

#if defined(ASIOEXT_USE_BOOST_ASIO)
# include <boost/asio/write.hpp>
#else
# include <asio/write.hpp>
#endif

#include <iostream>

ASIOEXT_NS_BEGIN

test_file_writer::test_file_writer(const char* filename,
                                   const void* data, std::size_t size)
  : filename_(filename)
{
  unique_file_handle fh = open(filename_,
                               open_flags::access_write |
                               open_flags::create_always);
  if (size != 0)
    asio::write(fh, asio::buffer(data, size));
}

test_file_writer::~test_file_writer()
{
  boost::system::error_code ec;
  boost::filesystem::remove(filename_, ec);
  if (ec) {
    std::cerr << "Failed to delete " << filename_ << " with " << ec;
    std::cerr << std::endl;
    std::abort();
  }
}

ASIOEXT_NS_END
