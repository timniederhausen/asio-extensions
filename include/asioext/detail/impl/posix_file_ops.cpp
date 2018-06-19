/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "asioext/open_args.hpp"

#include "asioext/detail/posix_file_ops.hpp"
#include "asioext/detail/chrono.hpp"
#include "asioext/detail/error.hpp"

#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif

#include <cerrno>

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h> // for off_t etc.
#include <sys/time.h> // for utimes

#if !defined(ASIOEXT_USE_FUTIMENS) && !defined(ASIOEXT_DISABLE_FUTIMENS)
# if defined(__linux__)
// __USE_XOPEN2K8 should be enough
#  if defined(__USE_XOPEN2K8)
#   define ASIOEXT_USE_FUTIMENS 1
#  endif
# elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
#  define ASIOEXT_USE_FUTIMENS 1
# endif
#endif

ASIOEXT_NS_BEGIN

static_assert(sizeof(int64_t) == sizeof(off_t), "off_t must be 64 bits");

namespace detail {
namespace posix_file_ops {

void set_error(error_code& ec, int e) ASIOEXT_NOEXCEPT
{
  ec = error_code(e, asio::error::get_system_category());
}

uint32_t file_attrs_to_native(file_attrs attrs) ASIOEXT_NOEXCEPT
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

file_attrs native_to_file_attrs(uint32_t native) ASIOEXT_NOEXCEPT
{
  file_attrs attrs = file_attrs::none;
#ifdef UF_HIDDEN
  if (native & UF_HIDDEN)
    attrs |= file_attrs::hidden;
#endif
#ifdef UF_SYSTEM
  if (native & UF_SYSTEM)
    attrs |= file_attrs::system;
#endif
#ifdef UF_ARCHIVE
  if (native & UF_ARCHIVE)
    attrs |= file_attrs::archive;
#endif
#ifdef UF_NODUMP
  if (native & UF_NODUMP)
    attrs |= file_attrs::no_dump;
#endif
#ifdef UF_IMMUTABLE
  if (native & UF_IMMUTABLE)
    attrs |= file_attrs::user_immutable;
#endif
#ifdef SF_IMMUTABLE
  if (native & SF_IMMUTABLE)
    attrs |= file_attrs::system_immutable;
#endif
#ifdef UF_NOUNLINK
  if (native & UF_NOUNLINK)
    attrs |= file_attrs::user_no_unlink;
#endif
#ifdef SF_NOUNLINK
  if (native & SF_NOUNLINK)
    attrs |= file_attrs::system_no_unlink;
#endif
  return attrs;
}

#if defined(__linux__) || defined(__FreeBSD__)
# define ASIOEXT_CTIME_SECONDS st.st_ctim.tv_sec
# define ASIOEXT_CTIME_NANOSECONDS st.st_ctim.tv_nsec
# define ASIOEXT_ATIME_SECONDS st.st_atim.tv_sec
# define ASIOEXT_ATIME_NANOSECONDS st.st_atim.tv_nsec
# define ASIOEXT_MTIME_SECONDS st.st_mtim.tv_sec
# define ASIOEXT_MTIME_NANOSECONDS st.st_mtim.tv_nsec
# if defined(__FreeBSD__) && !defined(_POSIX_SOURCE)
#  define ASIOEXT_BIRTHTIME_SECONDS st.st_birthtim.tv_sec
#  define ASIOEXT_BIRTHTIME_NANOSECONDS st.st_birthtim.tv_nsec
# else
#  define ASIOEXT_BIRTHTIME_SECONDS 0
#  define ASIOEXT_BIRTHTIME_NANOSECONDS 0
# endif
#elif defined(ANDROID)
# define ASIOEXT_CTIME_SECONDS st.st_ctime
# define ASIOEXT_CTIME_NANOSECONDS st.st_ctime_nsec
# define ASIOEXT_ATIME_SECONDS st.st_atime
# define ASIOEXT_ATIME_NANOSECONDS st.st_atime_nsec
# define ASIOEXT_MTIME_SECONDS st.st_mtime
# define ASIOEXT_MTIME_NANOSECONDS st.st_mtime_nsec
# define ASIOEXT_BIRTHTIME_SECONDS 0
# define ASIOEXT_BIRTHTIME_NANOSECONDS 0
#elif defined(__APPLE__) || defined(__NetBSD__) || defined(__OpenBSD__)
# define ASIOEXT_CTIME_SECONDS st.st_ctimespec.tv_sec
# define ASIOEXT_CTIME_NANOSECONDS st.st_ctimespec.tv_nsec
# define ASIOEXT_ATIME_SECONDS st.st_atimespec.tv_sec
# define ASIOEXT_ATIME_NANOSECONDS st.st_atimespec.tv_nsec
# define ASIOEXT_MTIME_SECONDS st.st_mtimespec.tv_sec
# define ASIOEXT_MTIME_NANOSECONDS st.st_mtimespec.tv_nsec
# define ASIOEXT_BIRTHTIME_SECONDS st.st_birthtimespec.tv_sec
# define ASIOEXT_BIRTHTIME_NANOSECONDS st.st_birthtimespec.tv_nsec
#else
# define ASIOEXT_CTIME_SECONDS st.st_ctime
# define ASIOEXT_CTIME_NANOSECONDS 0
# define ASIOEXT_ATIME_SECONDS st.st_atime
# define ASIOEXT_ATIME_NANOSECONDS 0
# define ASIOEXT_MTIME_SECONDS st.st_mtime
# define ASIOEXT_MTIME_NANOSECONDS 0
# define ASIOEXT_BIRTHTIME_SECONDS 0
# define ASIOEXT_BIRTHTIME_NANOSECONDS 0
#endif

bool stat_to_times(const struct stat& st,
                   file_time_type& ctime, file_time_type& atime,
                   file_time_type& mtime) ASIOEXT_NOEXCEPT
{
  file_time_type::duration ctim, atim, mtim;
  if (compose_time(chrono::seconds(ASIOEXT_BIRTHTIME_SECONDS),
                   chrono::nanoseconds(ASIOEXT_BIRTHTIME_NANOSECONDS),
                   ctim) &&
      compose_time(chrono::seconds(ASIOEXT_ATIME_SECONDS),
                   chrono::nanoseconds(ASIOEXT_ATIME_NANOSECONDS),
                   atim) &&
      compose_time(chrono::seconds(ASIOEXT_MTIME_SECONDS),
                   chrono::nanoseconds(ASIOEXT_MTIME_NANOSECONDS),
                   mtim)) {
    ctime = file_time_type(ctim);
    atime = file_time_type(atim);
    mtime = file_time_type(mtim);
    return true;
  }
  return false;
}

template <typename Seconds, typename Nanoseconds>
bool to_timespec(file_time_type t, Seconds& s,
                 Nanoseconds& ns) ASIOEXT_NOEXCEPT
{
  chrono::duration<Seconds, chrono::seconds::period> temp_s;
  chrono::duration<Nanoseconds, chrono::nanoseconds::period> temp_ns;
  if (decompose_time(t.time_since_epoch(), temp_s, temp_ns)) {
    s = temp_s.count();
    ns = temp_ns.count();
    return true;
  }
  return false;
}

template <typename Seconds, typename Microseconds>
bool to_timeval(file_time_type t, Seconds& s,
                Microseconds& us) ASIOEXT_NOEXCEPT
{
  chrono::duration<Seconds, chrono::seconds::period> temp_s;
  chrono::duration<Microseconds, chrono::microseconds::period> temp_us;
  if (decompose_time(t.time_since_epoch(), temp_s, temp_us)) {
    s = temp_s.count();
    us = temp_us.count();
    return true;
  }
  return false;
}

int parse_open_flags(open_flags flags) ASIOEXT_NOEXCEPT
{
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
  return native_flags;
}

handle_type open(const char* path, const open_args& args,
                 error_code& ec) ASIOEXT_NOEXCEPT
{
  mode_t mode = static_cast<mode_t>(args.mode());
  while (true) {
    handle_type fd = ::open(path, O_CLOEXEC | args.native_flags(), mode);
    if (fd != -1) {
      // Skip calling set_attributes() in case we don't support
      // attributes at all. TODO(tim): Fail in case attrs were requested?
#if ASIOEXT_HAS_FILE_FLAGS
      // Unfortunately there's no way to atomically set file
      // flags as part of the open() call.
      if (args.attrs() != 0 && ::fchflags(fd, args.attrs()) != 0) {
        set_error(ec, errno);
        ::close(fd);
        return -1;
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

void close(handle_type fd, error_code& ec) ASIOEXT_NOEXCEPT
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

handle_type duplicate(handle_type fd, error_code& ec) ASIOEXT_NOEXCEPT
{
  const int new_fd = ::dup(fd);
  if (new_fd != -1) {
    ec = error_code();
    return new_fd;
  }

  set_error(ec, errno);
  return -1;
}

handle_type get_stdin(error_code& ec) ASIOEXT_NOEXCEPT
{
  return STDIN_FILENO;
}

handle_type get_stdout(error_code& ec) ASIOEXT_NOEXCEPT
{
  return STDOUT_FILENO;
}

handle_type get_stderr(error_code& ec) ASIOEXT_NOEXCEPT
{
  return STDERR_FILENO;
}

uint64_t size(handle_type fd, error_code& ec) ASIOEXT_NOEXCEPT
{
  struct stat st;
  if (::fstat(fd, &st) == 0) {
    ec = error_code();
    return st.st_size;
  }

  set_error(ec, errno);
  return 0;
}

void size(handle_type fd, uint64_t new_size, error_code& ec) ASIOEXT_NOEXCEPT
{
  if (::ftruncate(fd, static_cast<off_t>(new_size)) == 0)
    ec = error_code();
  else
    set_error(ec, errno);
}

// Make sure our origin mappings match the system headers.
static_assert(static_cast<int>(seek_origin::from_begin) == SEEK_SET &&
              static_cast<int>(seek_origin::from_current) == SEEK_CUR &&
              static_cast<int>(seek_origin::from_end) == SEEK_END,
              "whence mapping must match the system headers");

uint64_t seek(handle_type fd, seek_origin origin, int64_t offset,
              error_code& ec) ASIOEXT_NOEXCEPT
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

file_perms permissions(handle_type fd, error_code& ec) ASIOEXT_NOEXCEPT
{
  struct stat st;
  if (::fstat(fd, &st) == 0) {
    ec = error_code();
    return static_cast<file_perms>(st.st_mode) & file_perms::all;
  }
  set_error(ec, errno);
  return file_perms::none;
}

void permissions(handle_type fd, file_perms perms, file_perm_options opts,
                 error_code& ec) ASIOEXT_NOEXCEPT
{
  mode_t mode = static_cast<mode_t>(perms & file_perms::all);
  if ((opts & (file_perm_options::add | file_perm_options::remove)) !=
      static_cast<file_perm_options>(0)) {
    struct stat st;
    if (::fstat(fd, &st) != 0) {
      set_error(ec, errno);
      return;
    }
    if ((opts & file_perm_options::add) != static_cast<file_perm_options>(0))
      mode |= st.st_mode;
    else
      mode = st.st_mode & ~mode;
  }

  if (::fchmod(fd, mode) == 0)
    ec = error_code();
  else
    set_error(ec, errno);
}

file_attrs attributes(handle_type fd, error_code& ec) ASIOEXT_NOEXCEPT
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

void attributes(handle_type fd, file_attrs attrs, file_attr_options opts,
                error_code& ec) ASIOEXT_NOEXCEPT
{
#if ASIOEXT_HAS_FILE_FLAGS
  uint32_t new_attrs = file_attrs_to_native(attrs);
  if ((opts & (file_attr_options::add | file_attr_options::remove)) !=
      static_cast<file_attr_options>(0)) {
    struct stat st;
    if (::fstat(fd, &st) != 0) {
      set_error(ec, errno);
      return;
    }
    if ((opts & file_attr_options::add) != static_cast<file_attr_options>(0))
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

void get_times(handle_type fd, file_time_type& ctime, file_time_type& atime,
               file_time_type& mtime, error_code& ec) ASIOEXT_NOEXCEPT
{
  struct stat st;
  if (::fstat(fd, &st) == 0) {
    if (stat_to_times(st, ctime, atime, mtime))
      ec = error_code();
    else
      ec = make_error_code(errc::value_too_large);
    return;
  }
  set_error(ec, errno);
}

void set_times(handle_type fd, file_time_type ctime, file_time_type atime,
               file_time_type mtime, error_code& ec) ASIOEXT_NOEXCEPT
{
#if defined(ASIOEXT_USE_FUTIMENS)
  struct ::timespec ts[2];

  if (atime.time_since_epoch().count() != 0) {
    if (!to_timespec(atime, ts[0].tv_sec, ts[0].tv_nsec)) {
      ec = make_error_code(errc::value_too_large);
      return;
    }
  } else {
    ts[0].tv_sec = 0;
    ts[0].tv_nsec = UTIME_OMIT;
  }

  if (mtime.time_since_epoch().count() != 0) {
    if (!to_timespec(mtime, ts[1].tv_sec, ts[1].tv_nsec)) {
      ec = make_error_code(errc::value_too_large);
      return;
    }
  } else {
    ts[1].tv_sec = 0;
    ts[1].tv_nsec = UTIME_OMIT;
  }

  if (::futimens(fd, ts) == 0) {
    ec = error_code();
    return;
  }
  set_error(ec, errno);
#else
  // XXX There's a race condition between ::fstat and ::futimes
  struct stat st;
  if (atime.time_since_epoch().count() == 0 ||
      mtime.time_since_epoch().count() == 0) {
    if (::fstat(fd, &st) != 0) {
      set_error(ec, errno);
      return;
    }
  }

  struct ::timeval tv[2];
  if (atime.time_since_epoch().count() != 0) {
    if (!to_timeval(atime, tv[0].tv_sec, tv[0].tv_usec)) {
      ec = make_error_code(errc::value_too_large);
      return;
    }
  } else {
    tv[0].tv_sec = ASIOEXT_ATIME_SECONDS;
    tv[0].tv_usec = chrono::duration_cast<chrono::microseconds>(
        chrono::nanoseconds(ASIOEXT_ATIME_NANOSECONDS)).count();
  }
  if (mtime.time_since_epoch().count() != 0) {
    if (!to_timeval(mtime, tv[1].tv_sec, tv[1].tv_usec)) {
      ec = make_error_code(errc::value_too_large);
      return;
    }
  } else {
    tv[1].tv_sec = ASIOEXT_MTIME_SECONDS;
    tv[1].tv_usec = chrono::duration_cast<chrono::microseconds>(
        chrono::nanoseconds(ASIOEXT_MTIME_NANOSECONDS)).count();
  }

  if (::futimes(fd, tv) == 0) {
    ec = error_code();
    return;
  }
  set_error(ec, errno);
#endif
}

std::size_t readv(handle_type fd, iovec* bufs, int count,
                  error_code& ec) ASIOEXT_NOEXCEPT
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

std::size_t writev(handle_type fd, const iovec* bufs, int count,
                   error_code& ec) ASIOEXT_NOEXCEPT
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

std::size_t pread(handle_type fd, void* buffer, std::size_t size,
                  uint64_t offset, error_code& ec) ASIOEXT_NOEXCEPT
{
  while (true) {
    const ssize_t r = ::pread(fd, buffer, size,
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
    if (size == 0) {
      ec = error_code();
      return 0;
    }
    ec = asio::error::eof;
    return 0;
  }
}

std::size_t pwrite(handle_type fd, const void* buffer, std::size_t size,
                   uint64_t offset, error_code& ec) ASIOEXT_NOEXCEPT
{
  while (true) {
    const ssize_t r = ::pwrite(fd, buffer, size,
                               static_cast<off_t>(offset));
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

#if defined(ASIOEXT_HAS_PVEC_IO_FUNCTIONS)
std::size_t preadv(handle_type fd, iovec* bufs, int count, uint64_t offset,
                   error_code& ec) ASIOEXT_NOEXCEPT
{
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

  for (int i = 0; i != count; ++i) {
    if (bufs[i].iov_len != 0) {
      ec = asio::error::eof;
      return 0;
    }
  }

  ec = error_code();
  return 0;
}

std::size_t pwritev(handle_type fd, const iovec* bufs, int count,
                    uint64_t offset, error_code& ec) ASIOEXT_NOEXCEPT
{
  while (true) {
    const ssize_t r = ::pwritev(fd, bufs, count, static_cast<off_t>(offset));
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
#endif

}
}

ASIOEXT_NS_END
