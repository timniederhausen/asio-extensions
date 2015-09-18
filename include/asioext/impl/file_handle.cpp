/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_IMPL_FILEHANDLE_CPP
#define ASIOEXT_IMPL_FILEHANDLE_CPP

#include "asioext/file_handle.hpp"

#include "asioext/detail/throw_error.hpp"

#include "asioext/detail/push_options.hpp"

ASIOEXT_NS_BEGIN

file_handle::file_handle(const native_handle_type& handle) ASIOEXT_NOEXCEPT
  : handle_(handle)
{
  // ctor
}

file_handle::~file_handle()
{
  asio::error_code ec;
  close(ec);
  // error is swallowed
}

void file_handle::open(const char* filename, uint32_t flags)
{
  asio::error_code ec;
  open(filename, flags, ec);
  detail::throw_error(ec);
}

#if defined(ASIOEXT_HAS_BOOST_FILESYSTEM)

void file_handle::open(const boost::filesystem::path& filename,
                       uint32_t flags)
{
  asio::error_code ec;
  open(filename, flags, ec);
  detail::throw_error(ec);
}

#endif

void file_handle::close()
{
  asio::error_code ec;
  close(ec);
  detail::throw_error(ec);
}

void file_handle::assign(const native_handle_type& handle)
{
  asio::error_code ec;
  assign(handle, ec);
  detail::throw_error(ec);
}

file_handle file_handle::duplicate()
{
  asio::error_code ec;
  file_handle h = duplicate(ec);
  detail::throw_error(ec);
  return h;
}

uint64_t file_handle::size()
{
  asio::error_code ec;
  uint64_t s = size(ec);
  detail::throw_error(ec);
  return s;
}

uint64_t file_handle::position()
{
  asio::error_code ec;
  uint64_t s = position(ec);
  detail::throw_error(ec);
  return s;
}

uint64_t file_handle::seek(seek_origin origin, int64_t offset)
{
  asio::error_code ec;
  uint64_t s = seek(origin, offset, ec);
  detail::throw_error(ec);
  return s;
}

ASIOEXT_NS_END

#include "asioext/detail/pop_options.hpp"

#endif
