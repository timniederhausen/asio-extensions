/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "asioext/open_flags.hpp"

#include "asioext/detail/win_file_ops.hpp"
#include "asioext/detail/error.hpp"

#include <windows.h>

ASIOEXT_NS_BEGIN

namespace detail {
namespace win_file_ops {

struct create_file_args
{
  DWORD creation_disposition;
  DWORD desired_access;
  DWORD share_mode;
  DWORD flags_and_attrs;
};

void set_error(error_code& ec)
{
  ec = error_code(::GetLastError(), asio::error::get_system_category());
}

bool parse_open_flags(create_file_args& args, uint32_t flags)
{
  if (!open_flags::is_valid(flags))
    return false;

  args.creation_disposition = 0;
  if (flags & open_flags::create_new)
    args.creation_disposition = CREATE_NEW;
  else if (flags & open_flags::create_always)
    args.creation_disposition = CREATE_ALWAYS;
  else if (flags & open_flags::open_existing)
    args.creation_disposition = OPEN_EXISTING;
  else if (flags & open_flags::open_always)
    args.creation_disposition = OPEN_ALWAYS;
  else if (flags & open_flags::truncate_existing)
    args.creation_disposition = TRUNCATE_EXISTING;

  args.desired_access = 0;
  if (flags & open_flags::access_read)
    args.desired_access |= GENERIC_READ;
  if (flags & open_flags::access_write)
    args.desired_access |= GENERIC_WRITE;

  args.flags_and_attrs = 0;
  if (flags & open_flags::attribute_hidden)
    args.flags_and_attrs |= FILE_ATTRIBUTE_HIDDEN;

  // TODO: Add support.
  args.share_mode = 0;
  return true;
}

handle_type open(const char* filename, uint32_t flags, error_code& ec)
{
  create_file_args args;
  if (!parse_open_flags(args, flags)) {
    ec = asio::error::invalid_argument;
    return INVALID_HANDLE_VALUE;
  }

  handle_type h =
      ::CreateFileA(filename, args.desired_access, args.share_mode, NULL,
                    args.creation_disposition, args.flags_and_attrs, NULL);

  if (h == INVALID_HANDLE_VALUE)
    set_error(ec);
  else
    ec = error_code();

  return h;
}

handle_type open(const wchar_t* filename, uint32_t flags, error_code& ec)
{
  create_file_args args;
  if (!parse_open_flags(args, flags)) {
    ec = asio::error::invalid_argument;
    return INVALID_HANDLE_VALUE;
  }

  const handle_type h =
      ::CreateFileW(filename, args.desired_access, args.share_mode, NULL,
                    args.creation_disposition, args.flags_and_attrs, NULL);

  if (h != INVALID_HANDLE_VALUE)
    ec = error_code();
  else
    set_error(ec);

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

  if (::DuplicateHandle(current_process, fd, current_process, &new_fd, 0, FALSE,
                        DUPLICATE_SAME_ACCESS))
    ec = error_code();
  else
    set_error(ec);

  return new_fd;
}

handle_type get_stdin(error_code& ec)
{
  const handle_type h = ::GetStdHandle(STD_INPUT_HANDLE);
  if (h != INVALID_HANDLE_VALUE)
    ec = error_code();
  else
    set_error(ec);

  return h;
}

handle_type get_stdout(error_code& ec)
{
  const handle_type h = ::GetStdHandle(STD_OUTPUT_HANDLE);
  if (h != INVALID_HANDLE_VALUE)
    ec = error_code();
  else
    set_error(ec);

  return h;
}

handle_type get_stderr(error_code& ec)
{
  const handle_type h = ::GetStdHandle(STD_ERROR_HANDLE);
  if (h != INVALID_HANDLE_VALUE)
    ec = error_code();
  else
    set_error(ec);

  return h;
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

uint64_t seek(handle_type fd, uint32_t origin, int64_t offset, error_code& ec)
{
  LARGE_INTEGER pos, res;
  pos.QuadPart = offset;

  if (::SetFilePointerEx(fd, pos, &res, origin))
    return res.QuadPart;

  set_error(ec);
  return 0;
}

uint32_t read(handle_type fd, void* buffer, uint32_t size, error_code& ec)
{
  DWORD bytesRead = 0;
  if (!::ReadFile(fd, buffer, size, &bytesRead, NULL)) {
    set_error(ec);
    return 0;
  }

  if (bytesRead == 0)
    ec = asio::error::eof;

  return bytesRead;
}

uint32_t write(handle_type fd,
               const void* buffer,
               uint32_t size,
               error_code& ec)
{
  DWORD bytesWritten = 0;
  if (!::WriteFile(fd, buffer, size, &bytesWritten, NULL)) {
    set_error(ec);
    return 0;
  }

  return bytesWritten;
}

uint32_t pread(handle_type fd,
               void* buffer,
               uint32_t size,
               uint64_t offset,
               error_code& ec)
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
                const void* buffer,
                uint32_t size,
                uint64_t offset,
                error_code& ec)
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
