/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "asioext/detail/posix_file_ops.hpp"
#include "asioext/detail/error.hpp"

#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif

#include <cerrno>

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h> // for off_t etc.

ASIOEXT_NS_BEGIN

static_assert(sizeof(int64_t) == sizeof(off_t), "off_t must be 64 bits");

namespace detail {
namespace posix_file_ops {

void set_error(error_code& ec, int e)
{
  ec = error_code(e, asio::error::get_system_category());
}

uint32_t file_attrs_to_native(file_attrs attrs)
{
  uint32_t native = 0;
#ifdef UF_HIDDEN
  if ((attrs & file_attrs::hidden) != file_attrs::none)
    native |= UF_HIDDEN;
#endif
#ifdef UF_SYSTEM
  if ((attrs & file_attrs::system) != file_attrs::none)
    native |= UF_SYSTEM;
#endif
#ifdef UF_ARCHIVE
  if ((attrs & file_attrs::archive) != file_attrs::none)
    native |= UF_ARCHIVE;
#endif
#ifdef UF_NODUMP
  if ((attrs & file_attrs::no_dump) != file_attrs::none)
    native |= UF_NODUMP;
#endif
#ifdef UF_IMMUTABLE
  if ((attrs & file_attrs::user_immutable) != file_attrs::none)
    native |= UF_IMMUTABLE;
#endif
#ifdef SF_IMMUTABLE
  if ((attrs & file_attrs::system_immutable) != file_attrs::none)
    native |= SF_IMMUTABLE;
#endif
#ifdef UF_NOUNLINK
  if ((attrs & file_attrs::user_no_unlink) != file_attrs::none)
    native |= UF_NOUNLINK;
#endif
#ifdef SF_NOUNLINK
  if ((attrs & file_attrs::system_no_unlink) != file_attrs::none)
    native |= SF_NOUNLINK;
#endif
  return native;
}

file_attrs native_to_file_attrs(uint32_t native)
{
  file_attrs attrs = file_attrs::none;
#ifdef UF_HIDDEN
  if (native & UF_HIDDEN)
    native |= file_attrs::hidden;
#endif
#ifdef UF_SYSTEM
  if (native & UF_SYSTEM)
    native |= file_attrs::system;
#endif
#ifdef UF_ARCHIVE
  if (native & UF_ARCHIVE)
    native |= file_attrs::archive;
#endif
#ifdef UF_NODUMP
  if (native & UF_NODUMP)
    native |= file_attrs::no_dump;
#endif
#ifdef UF_IMMUTABLE
  if (native & UF_IMMUTABLE)
    native |= file_attrs::user_immutable;
#endif
#ifdef SF_IMMUTABLE
  if (native & SF_IMMUTABLE)
    native |= file_attrs::system_immutable;
#endif
#ifdef UF_NOUNLINK
  if (native & UF_NOUNLINK)
    native |= file_attrs::user_no_unlink;
#endif
#ifdef SF_NOUNLINK
  if (native & SF_NOUNLINK)
    native |= file_attrs::system_no_unlink;
#endif
  return attrs;
}

handle_type open(const char* path, open_flags flags,
                 file_perms perms, file_attrs attrs,
                 error_code& ec)
{
  if (!is_valid(flags)) {
    ec = asio::error::invalid_argument;
    return -1;
  }

  int native_flags = 0;
  if ((flags & open_flags::create_new) != open_flags::none)
    native_flags = O_CREAT | O_EXCL;
  else if ((flags & open_flags::create_always) != open_flags::none)
    native_flags = O_CREAT | O_TRUNC;
  // else if ((flags & open_flags::open_existing) != open_flags::none)
  //  native_flags = 0;
  else if ((flags & open_flags::open_always) != open_flags::none)
    native_flags = O_CREAT;
  else if ((flags & open_flags::truncate_existing) != open_flags::none)
    native_flags = O_TRUNC;

  const open_flags rw_flags = flags & open_flags::access_read_write;
  switch (rw_flags) {
    case open_flags::access_read_write: native_flags |= O_RDWR; break;
    case open_flags::access_read: native_flags |= O_RDONLY; break;
    case open_flags::access_write: native_flags |= O_WRONLY; break;
  }

  mode_t mode = static_cast<mode_t>(perms & file_perms::all);
  while (true) {
    handle_type fd = ::open(path, O_CLOEXEC | native_flags, mode);
    if (fd != -1) {
      // Skip calling set_attributes() in case we don't support
      // attributes at all. TODO(tim): Fail in case attrs were requested?
#if ASIOEXT_HAS_FILE_FLAGS
      // Unfortunately there's no way to atomically set file
      // flags as part of the open() call.
      if (attrs != file_attrs::none) {
        attributes(fd, attrs, ec);
        if (ec) {
          ::close(fd);
          return -1;
        }
      }
#endif

      ec = error_code();
      return fd;
    }

    const int e = errno;
    if (e == EINTR)
      continue;

    set_error(ec, e);
    return -1;
  }
}

void close(handle_type fd, error_code& ec)
{
  // By the time close() returns, the fd is already gone
  // and could be re-used by another thread. Retrying the call would then
  // close someone elses' fd, which is certainly not what we want to do.
  // see: http://alobbs.com/post/54503240599/close-and-eintr
  // also: http://ewontfix.com/4/
  // TODO: This unfortunately doesn't apply to all UNIX systems...
  const int r = ::close(fd);

  if (r != -1)
    ec = error_code();
  else
    set_error(ec, errno);
}

handle_type duplicate(handle_type fd, error_code& ec)
{
  const int new_fd = ::dup(fd);
  if (new_fd != -1) {
    ec = error_code();
    return new_fd;
  }

  set_error(ec, errno);
  return -1;
}

handle_type get_stdin(error_code& ec)
{
  return STDIN_FILENO;
}

handle_type get_stdout(error_code& ec)
{
  return STDOUT_FILENO;
}

handle_type get_stderr(error_code& ec)
{
  return STDERR_FILENO;
}

uint64_t size(handle_type fd, error_code& ec)
{
  struct stat st;
  if (::fstat(fd, &st) != 0) {
    set_error(ec, errno);
    return 0;
  }

  ec = error_code();
  return st.st_size;
}

// Make sure our origin mappings match the system headers.
static_assert(static_cast<int>(seek_origin::from_begin) == SEEK_SET &&
              static_cast<int>(seek_origin::from_current) == SEEK_CUR &&
              static_cast<int>(seek_origin::from_end) == SEEK_END,
              "whence mapping must match the system headers");

uint64_t seek(handle_type fd, seek_origin origin, int64_t offset,
              error_code& ec)
{
  const off_t res = ::lseek(fd, static_cast<off_t>(offset),
                            static_cast<int>(origin));
  if (res != -1) {
    ec = error_code();
    return static_cast<uint64_t>(res);
  }

  set_error(ec, errno);
  return 0;
}

file_perms permissions(handle_type fd, error_code& ec)
{
  struct stat st;
  if (::fstat(fd, &st) == 0) {
    ec = error_code();
    return static_cast<file_perms>(st.st_mode) & file_perms::all;
  }
  set_error(ec, errno);
  return file_perms::none;
}

void permissions(handle_type fd, file_perms perms, error_code& ec)
{
  mode_t mode = static_cast<mode_t>(perms & file_perms::all);
  if ((perms & (file_perms::add_perms | file_perms::remove_perms)) !=
      file_perms::none) {
    struct stat st;
    if (::fstat(fd, &st) != 0) {
      set_error(ec, errno);
      return;
    }
    if ((perms & file_perms::add_perms) != file_perms::none)
      mode |= st.st_mode;
    else
      mode = st.st_mode & ~mode;
  }

  if (::fchmod(fd, mode) == 0)
    ec = error_code();
  else
    set_error(ec, errno);
}

file_attrs attributes(handle_type fd, error_code& ec)
{
#if ASIOEXT_HAS_FILE_FLAGS
  struct stat st;
  if (::fstat(fd, &st) == 0) {
    ec = error_code();
    return native_to_file_attrs(st.st_flags);
  }
  set_error(ec, errno);
#else
  // TODO(tim): Silently ignore everything?
  // Seems consistent.
  ec = error_code();
#endif
  return file_attrs::none;
}

void attributes(handle_type fd, file_attrs attrs, error_code& ec)
{
#if ASIOEXT_HAS_FILE_FLAGS
  uint32_t new_attrs = file_attrs_to_native(attrs);
  if ((attrs & (file_attrs::add_attrs | file_attrs::remove_attrs)) !=
      file_attrs::none) {
    struct stat st;
    if (::fstat(fd, &st) != 0) {
      set_error(ec, errno);
      return;
    }
    if ((attrs & file_attrs::add_attrs) != file_attrs::none)
      new_attrs = st.st_flags | new_attrs;
    else
      new_attrs = st.st_flags & ~new_attrs;
  }

  if (::fchflags(fd, new_attrs) == 0)
    ec = error_code();
  else
    set_error(ec, errno);
#else
  // TODO(tim): Silently ignore everything?
  // Seems consistent.
  ec = error_code();
#endif
}

std::size_t readv(handle_type fd, iovec* bufs, int count, error_code& ec)
{
  while (true) {
    const ssize_t r = ::readv(fd, bufs, count);
    if (r != 0) {
      if (r != -1) {
        ec = error_code();
        return static_cast<std::size_t>(r);
      }

      const int e = errno;
      if (e == EINTR)
        continue;

      set_error(ec, e);
      return 0;
    }
    break;
  }

  for (int i = 0; i != count; ++i) {
    if (bufs[i].iov_len != 0) {
      ec = asio::error::eof;
      return 0;
    }
  }

  ec = error_code();
  return 0;
}

std::size_t writev(handle_type fd, const iovec* bufs, int count, error_code& ec)
{
  while (true) {
    const ssize_t r = ::writev(fd, bufs, count);
    if (r != -1) {
      ec = error_code();
      return static_cast<std::size_t>(r);
    }

    const int e = errno;
    if (e == EINTR)
      continue;

    set_error(ec, e);
    return 0;
  }
}

std::size_t preadv(handle_type fd,
                   iovec* bufs,
                   int count,
                   uint64_t offset,
                   error_code& ec)
{
#if !defined(__APPLE__) && !defined(__MACH__)
  while (true) {
    const ssize_t r = ::preadv(fd, bufs, count, static_cast<off_t>(offset));
    if (r != 0) {
      if (r != -1) {
        ec = error_code();
        return static_cast<std::size_t>(r);
      }

      const int e = errno;
      if (e == EINTR)
        continue;

      set_error(ec, e);
      return 0;
    }
    break;
  }
#else
  if (count != 0) {
    while (true) {
      const ssize_t r = ::pread(fd, bufs[0].iov_base, bufs[0].iov_len,
                                static_cast<off_t>(offset));
      if (r != 0) {
        if (r != -1) {
          ec = error_code();
          return static_cast<std::size_t>(r);
        }

        const int e = errno;
        if (e == EINTR)
          continue;

        set_error(ec, e);
        return 0;
      }
      break;
    }
  }
#endif

  for (int i = 0; i != count; ++i) {
    if (bufs[i].iov_len != 0) {
      ec = asio::error::eof;
      return 0;
    }
  }

  ec = error_code();
  return 0;
}

std::size_t pwritev(handle_type fd,
                    const iovec* bufs,
                    int count,
                    uint64_t offset,
                    error_code& ec)
{
  while (true) {
#if !defined(__APPLE__) && !defined(__MACH__)
    const ssize_t r = ::pwritev(fd, bufs, count, static_cast<off_t>(offset));
#else
    const ssize_t r = ::pwrite(fd, bufs[0].iov_base, bufs[0].iov_len,
                               static_cast<off_t>(offset));
#endif

    if (r != -1) {
      ec = error_code();
      return static_cast<std::size_t>(r);
    }

    const int e = errno;
    if (e == EINTR)
      continue;

    set_error(ec, e);
    return 0;
  }
}

}
}

ASIOEXT_NS_END
