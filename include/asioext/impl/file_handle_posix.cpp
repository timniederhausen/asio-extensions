/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "asioext/file_handle.hpp"

#include "asioext/detail/posix_file_ops.hpp"
#include "asioext/detail/throw_error.hpp"

ASIOEXT_NS_BEGIN

file_handle::file_handle() ASIOEXT_NOEXCEPT
  : handle_(-1)
{
  // ctor
}

bool file_handle::is_open() const ASIOEXT_NOEXCEPT
{
  return handle_ != -1;
}

void file_handle::close(error_code& ec) ASIOEXT_NOEXCEPT
{
  if (handle_ == -1) {
    ec = error_code();
    return;
  }

  detail::posix_file_ops::close(handle_, ec);
  handle_ = -1;
}

void file_handle::clear() ASIOEXT_NOEXCEPT
{
  handle_ = -1;
}

uint64_t file_handle::position(error_code& ec) ASIOEXT_NOEXCEPT
{
  return seek(seek_origin::from_current, 0, ec);
}

uint64_t file_handle::seek(seek_origin origin,
                           int64_t offset,
                           error_code& ec) ASIOEXT_NOEXCEPT
{
  return detail::posix_file_ops::seek(handle_, origin, offset, ec);
}

uint64_t file_handle::size(error_code& ec) ASIOEXT_NOEXCEPT
{
  return detail::posix_file_ops::size(handle_, ec);
}

void file_handle::size(uint64_t new_size, error_code& ec) ASIOEXT_NOEXCEPT
{
  detail::posix_file_ops::size(handle_, new_size, ec);
}

file_perms file_handle::permissions(error_code& ec) ASIOEXT_NOEXCEPT
{
  return detail::posix_file_ops::permissions(handle_, ec);
}

void file_handle::permissions(file_perms perms, file_perm_options opts,
                              error_code& ec) ASIOEXT_NOEXCEPT
{
  detail::posix_file_ops::permissions(handle_, perms, opts, ec);
}

file_attrs file_handle::attributes(error_code& ec) ASIOEXT_NOEXCEPT
{
  return detail::posix_file_ops::attributes(handle_, ec);
}

void file_handle::attributes(file_attrs attrs, file_attr_options opts,
                             error_code& ec) ASIOEXT_NOEXCEPT
{
  detail::posix_file_ops::attributes(handle_, attrs, opts, ec);
}

file_times file_handle::times(error_code& ec) ASIOEXT_NOEXCEPT
{
  file_times res;
  detail::posix_file_ops::get_times(handle_, res.ctime, res.atime,
                                    res.mtime, ec);
  return res;
}

void file_handle::times(const file_times& new_times,
                        error_code& ec) ASIOEXT_NOEXCEPT
{
  detail::posix_file_ops::set_times(handle_, new_times.ctime, new_times.atime,
                                    new_times.mtime, ec);
}

ASIOEXT_NS_END
