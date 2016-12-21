/// @copyright Copyright (c) 2016 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_IMPL_READFILE_CPP
#define ASIOEXT_IMPL_READFILE_CPP

#include "asioext/read_file.hpp"
#include "asioext/scoped_file_handle.hpp"
#include "asioext/open_flags.hpp"

#include "asioext/detail/error.hpp"
#include "asioext/detail/throw_error.hpp"

#if defined(ASIOEXT_USE_BOOST_ASIO)
# include <boost/asio/read.hpp>
#else
# include <asio/read.hpp>
#endif

ASIOEXT_NS_BEGIN

ASIOEXT_DECL void read_file(const char* filename, std::string& str)
{
  error_code ec;
  read_file(filename, str, ec);
  detail::throw_error(ec);
}

ASIOEXT_DECL void read_file(const char* filename,
                            std::string& str,
                            error_code& ec)
{
  scoped_file_handle file(filename, open_flags::access_read |
                                    open_flags::open_existing,
                            ec);
  if (!ec)
    read_file(file.get(), str, ec);
}

#if defined(ASIOEXT_WINDOWS)
void read_file(const wchar_t* filename, std::string& str)
{
  error_code ec;
  read_file(filename, str, ec);
  detail::throw_error(ec);
}

void read_file(const wchar_t* filename, std::string& str, error_code& ec)
{
  scoped_file_handle file(filename, open_flags::access_read |
                                    open_flags::open_existing,
                            ec);
  if (!ec)
    read_file(file.get(), str, ec);
}
#endif

#if defined(ASIOEXT_HAS_BOOST_FILESYSTEM)
ASIOEXT_DECL void read_file(const boost::filesystem::path& filename,
                            std::string& str)
{
  error_code ec;
  read_file(filename, str, ec);
  detail::throw_error(ec);
}

ASIOEXT_DECL void read_file(const boost::filesystem::path& filename,
                            std::string& str,
                            error_code& ec)
{
  scoped_file_handle file(filename, open_flags::access_read |
                                    open_flags::open_existing,
                            ec);
  if (!ec)
    read_file(file.get(), str, ec);
}
#endif

ASIOEXT_DECL void read_file(file_handle file, std::string& str)
{
  error_code ec;
  read_file(file, str, ec);
  detail::throw_error(ec);
}

ASIOEXT_DECL void read_file(file_handle file, std::string& str, error_code& ec)
{
  const uint64_t size = file.size(ec);
  if (ec) return;

  if (size > std::numeric_limits<std::string::size_type>::max()) {
    ec = asio::error::message_size;
    return;
  }

  if (size != 0) {
    str.resize(static_cast<std::string::size_type>(size));
    asio::read(file, asio::buffer(&str[0], str.size()), ec);
  } else {
    str.clear();
  }
}

ASIOEXT_NS_END

#endif
