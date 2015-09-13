/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_DETAIL_IMPL_POSIXFILEOPS_CPP
#define ASIOEXT_DETAIL_IMPL_POSIXFILEOPS_CPP

#include "asioext/detail/posix_file_ops.hpp"

#include <asio/error.hpp>

#include <cerrno>
#include <fcntl.h>

#include "asioext/detail/push_options.hpp"

ASIOEXT_NS_BEGIN

namespace detail {
namespace posix_file_ops {

void set_error(asio::error_code& ec, int e)
{
  ec.assign(e, asio::error::get_system_category());
}

handle_type open(const char* path, int flags, asio::error_code& ec)
{
  while (true) {
    handle_type fd = ::open(path, flags);
    if (fd != -1) {
      ec.clear();
      return fd;
    }

    const int e = errno;
    if (e == EINTR)
      continue;

    set_error(ec, e);
    return -1;
  }
}

void close(handle_type fd, asio::error_code& ec)
{
  // By the time close() returns, the fd is already gone
  // and could be re-used by another thread. Retrying the call would then
  // close someone elses' fd, which is certainly not what we want to do.
  // see: http://alobbs.com/post/54503240599/close-and-eintr
  const int r = ::close(fd);

  if (r != -1)
    ec.clear();
  else
    set_error(ec, errno);
}

std::size_t sync_readv(handle_type fd, iovec* bufs, int count,
                       asio::error_code& ec)
{
  while (true) {
    const ssize_t r = ::readv(fd, bufs, count);

    if (r == 0) {
      ec = asio::error::eof;
      return 0;
    }

    if (r != -1) {
      ec.clear();
      return static_cast<std::size_t>(r);
    }

    const int e = errno;
    if (e == EINTR)
      continue;

    set_error(ec, e);
    return 0;
  }
}

std::size_t sync_writev(handle_type fd, const iovec* bufs, int count,
                        asio::error_code& ec)
{
  while (true) {
    const ssize_t r = ::writev(fd, bufs, count);
    if (r != -1) {
      ec.clear();
      return static_cast<std::size_t>(r);
    }

    const int e = errno;
    if (e == EINTR)
      continue;

    set_error(ec, e);
    return 0;
  }
}

std::size_t sync_preadv(handle_type fd, iovec* bufs, int count,
                        off_t offset, asio::error_code& ec)
{
  while (true) {
    const ssize_t r = ::preadv(fd, bufs, count, offset);

    if (r == 0) {
      ec = asio::error::eof;
      return 0;
    }

    if (r != -1) {
      ec.clear();
      return static_cast<std::size_t>(r);
    }

    const int e = errno;
    if (e == EINTR)
      continue;

    set_error(ec, e);
    return 0;
  }
}

std::size_t sync_pwritev(handle_type fd, const iovec* bufs, int count,
                         off_t offset, asio::error_code& ec)
{
  while (true) {
    const ssize_t r = ::pwritev(fd, bufs, count, offset);
    if (r != -1) {
      ec.clear();
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

#include "asioext/detail/pop_options.hpp"

#endif
