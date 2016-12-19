/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "asioext/file_handle.hpp"

#include "asioext/detail/throw_error.hpp"

ASIOEXT_NS_BEGIN

file_handle::file_handle(const native_handle_type& handle) ASIOEXT_NOEXCEPT
  : handle_(handle)
{
  // ctor
}

file_handle::~file_handle()
{
}

void file_handle::close()
{
  error_code ec;
  close(ec);
  detail::throw_error(ec);
}

uint64_t file_handle::size()
{
  error_code ec;
  uint64_t s = size(ec);
  detail::throw_error(ec);
  return s;
}

uint64_t file_handle::position()
{
  error_code ec;
  uint64_t s = position(ec);
  detail::throw_error(ec);
  return s;
}

uint64_t file_handle::seek(seek_origin origin, int64_t offset)
{
  error_code ec;
  uint64_t s = seek(origin, offset, ec);
  detail::throw_error(ec);
  return s;
}

ASIOEXT_NS_END
