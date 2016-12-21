/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "asioext/detail/posix_file_ops.hpp"
#include "asioext/detail/error.hpp"

#include "asioext/open_flags.hpp"

#define _FILE_OFFSET_BITS 64

#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h> // for off_t etc.

ASIOEXT_NS_BEGIN

namespace detail {
namespace posix_file_ops {

void set_error(error_code& ec, int e)
{
  ec = error_code(e, asio::error::get_system_category());
}

handle_type open(const char* path, uint32_t flags, error_code& ec)
{
  if (!open_flags::is_valid(flags)) {
    ec = asio::error::invalid_argument;
    return -1;
  }

  int native_flags = O_CLOEXEC;

  if (flags & open_flags::create_new)
    native_flags = O_CREAT | O_EXCL;
  else if (flags & open_flags::create_always)
    native_flags = O_CREAT | O_TRUNC;
  // else if (flags & open_flags::open_existing)
  //  native_flags = 0;
  else if (flags & open_flags::open_always)
    native_flags = O_CREAT;
  else if (flags & open_flags::truncate_existing)
    native_flags = O_TRUNC;

  const uint32_t rw_flags = flags & open_flags::access_rw;
  switch (rw_flags) {
    case open_flags::access_rw: native_flags |= O_RDWR; break;
    case open_flags::access_read: native_flags |= O_RDONLY; break;
    case open_flags::access_write: native_flags |= O_WRONLY; break;
  }

  int mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
  if (flags & open_flags::attribute_executable)
    mode |= S_IXGRP | S_IXOTH | S_IXUSR;

  while (true) {
    handle_type fd = ::open(path, native_flags, mode);
    if (fd != -1) {
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
  if (fstat(fd, &st) != 0) {
    set_error(ec, errno);
    return 0;
  }

  return st.st_size;
}

uint64_t seek(handle_type fd, int origin, int64_t offset, error_code& ec)
{
  const off_t res =
      ::lseek(fd, static_cast<off_t>(offset), static_cast<int>(origin));

  if (res != -1) {
    ec = error_code();
    return static_cast<uint64_t>(res);
  }

  set_error(ec, errno);
  return 0;
}

std::size_t readv(handle_type fd, iovec* bufs, int count, error_code& ec)
{
  const ssize_t r = ::readv(fd, bufs, count);

  // @todo Only return EOF if the buffers have non-zero size?
  if (r == 0) {
    ec = asio::error::eof;
    return 0;
  }

  if (r != -1) {
    ec = error_code();
    return static_cast<std::size_t>(r);
  }

  set_error(ec, errno);
  return 0;
}

std::size_t writev(handle_type fd, const iovec* bufs, int count, error_code& ec)
{
  const ssize_t r = ::writev(fd, bufs, count);
  if (r != -1) {
    ec = error_code();
    return static_cast<std::size_t>(r);
  }

  set_error(ec, errno);
  return 0;
}

std::size_t preadv(handle_type fd,
                   iovec* bufs,
                   int count,
                   off_t offset,
                   error_code& ec)
{
  const ssize_t r = ::preadv(fd, bufs, count, offset);

  // @todo Only return EOF if the buffers have non-zero size?
  if (r == 0) {
    ec = asio::error::eof;
    return 0;
  }

  if (r != -1) {
    ec = error_code();
    return static_cast<std::size_t>(r);
  }

  set_error(ec, errno);
  return 0;
}

std::size_t pwritev(handle_type fd,
                    const iovec* bufs,
                    int count,
                    off_t offset,
                    error_code& ec)
{
  const ssize_t r = ::pwritev(fd, bufs, count, offset);
  if (r != -1) {
    ec = error_code();
    return static_cast<std::size_t>(r);
  }

  set_error(ec, errno);
  return 0;
}

}
}

ASIOEXT_NS_END
