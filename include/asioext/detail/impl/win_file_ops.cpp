/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_DETAIL_IMPL_WINFILEOPS_CPP
#define ASIOEXT_DETAIL_IMPL_WINFILEOPS_CPP

#include "asioext/detail/win_file_ops.hpp"
#include "asioext/open_flags.hpp"

#include <asio/error.hpp>

#include <windows.h>

#include "asioext/detail/push_options.hpp"

ASIOEXT_NS_BEGIN

namespace detail {
namespace win_file_ops {

void set_error(asio::error_code& ec)
{
  ec.assign(GetLastError(), asio::error::get_system_category());
}

handle_type open(const wchar_t* filename, uint32_t flags, asio::error_code& ec)
{
  if (!open_flags::is_valid(flags)) {
    ec = asio::error::invalid_argument;
    return INVALID_HANDLE_VALUE;
  }

  DWORD creationDisposition = 0;
  if (flags & open_flags::create_new)
    creationDisposition = CREATE_NEW;
  else if (flags & open_flags::create_always)
    creationDisposition = CREATE_ALWAYS;
  else if (flags & open_flags::open_existing)
    creationDisposition = OPEN_EXISTING;
  else if (flags & open_flags::open_always)
    creationDisposition = OPEN_ALWAYS;
  else if (flags & open_flags::truncate_existing)
    creationDisposition = TRUNCATE_EXISTING;

  DWORD desiredAccess = 0;
  if (flags & open_flags::access_read)
    desiredAccess |= GENERIC_READ;
  if (flags & open_flags::access_write)
    desiredAccess |= GENERIC_WRITE;

  // TODO: Add support for these
  DWORD shareMode = 0;
  DWORD flagsAndAttributes = 0;

  handle_type h = ::CreateFileW(filename, desiredAccess, shareMode,
                                nullptr, creationDisposition,
                                flagsAndAttributes, nullptr);

  if (h == INVALID_HANDLE_VALUE)
    set_error(ec);
  else
    ec.clear();

  return h;
}

uint32_t read(handle_type fd,
              void* buffer, uint32_t size,
              asio::error_code& ec)
{
  DWORD bytesRead = 0;
  if (!::ReadFile(fd, buffer, size, &bytesRead, nullptr)) {
    set_error(ec);
    return 0;
  }

  if (bytesRead == 0)
    ec = asio::error::eof;

  return bytesRead;
}

uint32_t write(handle_type fd,
               const void* buffer, uint32_t size,
               asio::error_code& ec)
{
  DWORD bytesWritten = 0;
  if (!::WriteFile(fd, buffer, size, &bytesWritten, nullptr)) {
    set_error(ec);
    return 0;
  }

  return bytesWritten;
}

uint32_t pread(handle_type fd,
               void* buffer, uint32_t size,
               uint64_t offset, asio::error_code& ec)
{
  LARGE_INTEGER offset2;
  offset2.QuadPart = offset;

  OVERLAPPED overlapped = {0};
  overlapped.Offset = offset2.LowPart;
  overlapped.OffsetHigh = offset2.HighPart;

  DWORD bytesRead = 0;
  if (!::ReadFile(fd, buffer, size, &bytesRead, &overlapped)) {
    set_error(ec);
    return 0;
  }

  if (bytesRead == 0)
    ec = asio::error::eof;

  return bytesRead;
}

uint32_t pwrite(handle_type fd,
                const void* buffer, uint32_t size,
                uint64_t offset, asio::error_code& ec)
{
  LARGE_INTEGER offset2;
  offset2.QuadPart = offset;

  OVERLAPPED overlapped = {0};
  overlapped.Offset = offset2.LowPart;
  overlapped.OffsetHigh = offset2.HighPart;

  DWORD bytesWritten = 0;
  if (!::WriteFile(fd, buffer, size, &bytesWritten, &overlapped)) {
    set_error(ec);
    return 0;
  }

  return bytesWritten;
}

}
}

ASIOEXT_NS_END

#include "asioext/detail/pop_options.hpp"

#endif
