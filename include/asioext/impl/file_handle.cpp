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

uint64_t file_handle::size()
{
  error_code ec;
  uint64_t s = size(ec);
  detail::throw_error(ec);
  return s;
}

void file_handle::size(uint64_t new_size)
{
  error_code ec;
  size(new_size, ec);
  detail::throw_error(ec, "set_size");
}

#if defined(ASIOEXT_MSVC) && (ASIOEXT_MSVC >= 1400) \
  && (!defined(_WIN32_WINNT) || _WIN32_WINNT < 0x0600)
#pragma warning(push)
#pragma warning(disable:4996)
#endif

file_perms file_handle::permissions()
{
  error_code ec;
  file_perms p = permissions(ec);
  detail::throw_error(ec, "get_permissions");
  return p;
}

void file_handle::permissions(file_perms perms,
                              file_perm_options opts)
{
  error_code ec;
  permissions(perms, opts, ec);
  detail::throw_error(ec, "set_permissions");
}

void file_handle::permissions(file_perms perms,
                              error_code& ec) ASIOEXT_NOEXCEPT
{
  permissions(perms, file_perm_options::replace, ec);
}

file_attrs file_handle::attributes()
{
  error_code ec;
  file_attrs a = attributes(ec);
  detail::throw_error(ec, "get_attributes");
  return a;
}

void file_handle::attributes(file_attrs attrs,
                             file_attr_options opts)
{
  error_code ec;
  attributes(attrs, opts, ec);
  detail::throw_error(ec, "set_attributes");
}

void file_handle::attributes(file_attrs attrs,
                             error_code& ec) ASIOEXT_NOEXCEPT
{
  attributes(attrs, file_attr_options::replace, ec);
}

#if defined(ASIOEXT_MSVC) && (ASIOEXT_MSVC >= 1400) \
  && (!defined(_WIN32_WINNT) || _WIN32_WINNT < 0x0600)
#pragma warning(pop)
#endif

file_times file_handle::times()
{
  error_code ec;
  file_times t = times(ec);
  detail::throw_error(ec, "times");
  return t;
}

void file_handle::times(const file_times& new_times)
{
  error_code ec;
  times(new_times, ec);
  detail::throw_error(ec, "times");
}

ASIOEXT_NS_END
