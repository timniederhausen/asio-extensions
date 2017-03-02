/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "asioext/file_handle.hpp"

#include "asioext/detail/win_file_ops.hpp"
#include "asioext/detail/win_path.hpp"
#include "asioext/detail/throw_error.hpp"

#include <windows.h>

ASIOEXT_NS_BEGIN

file_handle::file_handle() ASIOEXT_NOEXCEPT
  : handle_(INVALID_HANDLE_VALUE)
{
  // ctor
}

bool file_handle::is_open() const ASIOEXT_NOEXCEPT
{
  return handle_ != INVALID_HANDLE_VALUE;
}

void file_handle::close(error_code& ec) ASIOEXT_NOEXCEPT
{
  if (handle_ == INVALID_HANDLE_VALUE) {
    ec = error_code();
    return;
  }

  detail::win_file_ops::close(handle_, ec);
  handle_ = INVALID_HANDLE_VALUE;
}

void file_handle::clear() ASIOEXT_NOEXCEPT
{
  handle_ = INVALID_HANDLE_VALUE;
}

uint64_t file_handle::size(error_code& ec) ASIOEXT_NOEXCEPT
{
  return detail::win_file_ops::size(handle_, ec);
}

uint64_t file_handle::position(error_code& ec) ASIOEXT_NOEXCEPT
{
  return seek(seek_origin::from_current, 0, ec);
}

uint64_t file_handle::seek(seek_origin origin,
                           int64_t offset,
                           error_code& ec) ASIOEXT_NOEXCEPT
{
  return detail::win_file_ops::seek(handle_, static_cast<int>(origin),
                                    offset, ec);
}

ASIOEXT_NS_END
