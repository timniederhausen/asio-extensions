/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_DETAIL_IMPL_WINFILEOPS_CPP
#define ASIOEXT_DETAIL_IMPL_WINFILEOPS_CPP

#include "asioext/open_flags.hpp"

#include "asioext/detail/win_file_ops.hpp"
#include "asioext/detail/error.hpp"

#include <windows.h>

ASIOEXT_NS_BEGIN

namespace detail {
namespace win_file_ops {

void set_error(error_code& ec)
{
  ec = error_code(::GetLastError(),
                        asio::error::get_system_category());
}

handle_type open(const wchar_t* filename, uint32_t flags, error_code& ec)
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
    ec = error_code();

  return h;
}

void close(handle_type fd, error_code& ec)
{
  if (::CloseHandle(fd))
    ec = error_code();
  else
    set_error(ec);
}

handle_type duplicate(handle_type fd, error_code& ec)
{
  const handle_type current_process = ::GetCurrentProcess();
  handle_type new_fd = INVALID_HANDLE_VALUE;

  if (::DuplicateHandle(current_process, fd, current_process, &new_fd,
      0, FALSE, DUPLICATE_SAME_ACCESS))
    ec = error_code();
  else
    set_error(ec);

  return new_fd;
}

uint64_t size(handle_type fd, error_code& ec)
{
  LARGE_INTEGER size;
  if (::GetFileSizeEx(fd, &size)) {
    ec = error_code();
    return static_cast<uint64_t>(size.QuadPart);
  }

  set_error(ec);
  return 0;
}

uint64_t seek(handle_type fd, uint32_t origin, int64_t offset,
              error_code& ec)
{
  LARGE_INTEGER pos, res;
  pos.QuadPart = offset;

  if (::SetFilePointerEx(fd, pos, &res, origin))
    return res.QuadPart;

  set_error(ec);
  return 0;
}

uint32_t read(handle_type fd,
              void* buffer, uint32_t size,
              error_code& ec)
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
               error_code& ec)
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
               uint64_t offset, error_code& ec)
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
                uint64_t offset, error_code& ec)
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

#endif
