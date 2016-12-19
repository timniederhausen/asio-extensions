/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "asioext/detail/throw_error.hpp"

#include "asioext/scoped_file_handle.hpp"
#include "asioext/open.hpp"

ASIOEXT_NS_BEGIN

scoped_file_handle::scoped_file_handle() ASIOEXT_NOEXCEPT
  : handle_()
{
  // ctor
}

scoped_file_handle::scoped_file_handle(file_handle handle) ASIOEXT_NOEXCEPT
  : handle_(handle)
{
  // ctor
}

scoped_file_handle::~scoped_file_handle()
{
  error_code ec;
  close(ec);
  // error is swallowed
}

#ifdef ASIOEXT_HAS_MOVE

scoped_file_handle::scoped_file_handle(scoped_file_handle&& other) ASIOEXT_NOEXCEPT
  : handle_(other.handle_)
{
  other.handle_.clear();
}

scoped_file_handle& scoped_file_handle::operator=(scoped_file_handle&& other)
{
  if (handle_.is_open())
    handle_.close();

  handle_ = other.handle_;
  other.handle_.clear();
  return *this;
}

#endif

void scoped_file_handle::open(const char* filename, uint32_t flags)
{
  error_code ec;
  open(filename, flags, ec);
  detail::throw_error(ec);
}

void scoped_file_handle::open(const char* filename,
                              uint32_t flags,
                              error_code& ec) ASIOEXT_NOEXCEPT
{
  if (handle_.is_open()) {
    handle_.close(ec);
    if (ec) return;
  }

  handle_ = asioext::open(filename, flags, ec);
}

#if defined(ASIOEXT_HAS_BOOST_FILESYSTEM)

void scoped_file_handle::open(const boost::filesystem::path& filename,
                              uint32_t flags)
{
  error_code ec;
  open(filename, flags, ec);
  detail::throw_error(ec);
}

void scoped_file_handle::open(const boost::filesystem::path& filename,
                              uint32_t flags,
                              error_code& ec) ASIOEXT_NOEXCEPT
{
  if (handle_.is_open()) {
    handle_.close(ec);
    if (ec) return;
  }

  handle_ = asioext::open(filename, flags, ec);
}

#endif

file_handle scoped_file_handle::leak() ASIOEXT_NOEXCEPT
{
  file_handle handle = handle_;
  handle_.clear();
  return handle;
}

void scoped_file_handle::reset(file_handle handle)
{
  error_code ec;
  reset(handle, ec);
  detail::throw_error(ec);
}

void scoped_file_handle::reset(file_handle handle,
                               error_code& ec) ASIOEXT_NOEXCEPT
{
  close(ec);
  if (!ec)
    handle_ = handle;
}

ASIOEXT_NS_END
