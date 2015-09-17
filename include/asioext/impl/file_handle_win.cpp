/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_IMPL_FILEHANDLE_WIN_CPP
#define ASIOEXT_IMPL_FILEHANDLE_WIN_CPP

#include "asioext/file_handle.hpp"

#include "asioext/detail/win_file_ops.hpp"
#include "asioext/detail/win_path.hpp"
#include "asioext/detail/throw_error.hpp"

#include <asio/error.hpp>

#include <windows.h>

#include "asioext/detail/push_options.hpp"

ASIOEXT_NS_BEGIN

file_handle::file_handle() ASIOEXT_NOEXCEPT
  : handle_(INVALID_HANDLE_VALUE)
{
  // ctor
}

file_handle::file_handle(const char* filename, uint32_t flags)
  : handle_(INVALID_HANDLE_VALUE)
{
  open(filename, flags);
}

file_handle::file_handle(const char* filename, uint32_t flags,
                         asio::error_code& ec) ASIOEXT_NOEXCEPT
  : handle_(INVALID_HANDLE_VALUE)
{
  open(filename, flags, ec);
}

#if defined(ASIOEXT_HAS_BOOST_FILESYSTEM)

file_handle::file_handle(const boost::filesystem::path& filename,
                         uint32_t flags)
  : handle_(INVALID_HANDLE_VALUE)
{
  open(filename, flags);
}

file_handle::file_handle(const boost::filesystem::path& filename,
                         uint32_t flags,
                         asio::error_code& ec) ASIOEXT_NOEXCEPT
  : handle_(INVALID_HANDLE_VALUE)
{
  open(filename, flags, ec);
}

#endif

#ifdef ASIOEXT_HAS_MOVE

file_handle::file_handle(file_handle&& other) ASIOEXT_NOEXCEPT
  : handle_(other.handle_)
{
  other.handle_ = INVALID_HANDLE_VALUE;
}

file_handle& file_handle::operator=(file_handle&& other)
{
  if (handle_ != INVALID_HANDLE_VALUE)
    close();

  handle_ = other.handle_;
  other.handle_ = INVALID_HANDLE_VALUE;
  return *this;
}

#endif

void file_handle::open(const char* filename, uint32_t flags,
                       asio::error_code& ec) ASIOEXT_NOEXCEPT
{
  detail::win_path p(filename, std::strlen(filename), ec);
  if (ec)
    return;

  handle_ = detail::win_file_ops::open(p.c_str(), flags, ec);
}

#if defined(ASIOEXT_HAS_BOOST_FILESYSTEM)

void file_handle::open(const boost::filesystem::path& filename,
                       uint32_t flags,
                       asio::error_code& ec) ASIOEXT_NOEXCEPT
{
  handle_ = detail::win_file_ops::open(filename.c_str(), flags, ec);
}

#endif

bool file_handle::is_open() const ASIOEXT_NOEXCEPT
{
  return handle_ != INVALID_HANDLE_VALUE;
}

void file_handle::close(asio::error_code& ec) ASIOEXT_NOEXCEPT
{
  if (handle_ == INVALID_HANDLE_VALUE) {
    ec.clear();
    return;
  }

  detail::win_file_ops::close(handle_, ec);
  handle_ = INVALID_HANDLE_VALUE;
}

void file_handle::assign(const native_handle_type& handle,
                         asio::error_code& ec) ASIOEXT_NOEXCEPT
{
  close(ec);
  if (ec)
    return;

  handle_ = handle;
}

uint64_t file_handle::size(asio::error_code& ec) ASIOEXT_NOEXCEPT
{
  return detail::win_file_ops::size(handle_, ec);
}

uint64_t file_handle::position(asio::error_code& ec) ASIOEXT_NOEXCEPT
{
  return seek(from_current, 0, ec);
}

uint64_t file_handle::seek(seek_origin origin, int64_t offset,
                           asio::error_code& ec) ASIOEXT_NOEXCEPT
{
  return detail::win_file_ops::seek(handle_, origin, offset, ec);
}

ASIOEXT_NS_END

#include "asioext/detail/pop_options.hpp"

#endif
