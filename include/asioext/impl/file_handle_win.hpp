/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_IMPL_FILEHANDLE_WIN_HPP
#define ASIOEXT_IMPL_FILEHANDLE_WIN_HPP

#include "asioext/file_handle.hpp"

#include "asioext/detail/win_file_ops.hpp"
#include "asioext/detail/throw_error.hpp"

#include <asio/buffer.hpp>

#include "asioext/detail/push_options.hpp"

ASIOEXT_NS_BEGIN

// Note: Currently always only the first buffer is used.
// This is possible since we can get away with returning less than requested
// and is unfortunately the only way this can be implemented reasonably.
//
// Windows has vectored I/O functions (ReadFileScatter, WriteFileGather),
// but they impose very strict alignment and size requirements and thus are
// unusable here.
//
// @todo Skip zero-length buffers (?)

template <typename MutableBufferSequence>
std::size_t file_handle::read_some(const MutableBufferSequence& buffers)
{
  asio::error_code ec;
  std::size_t s = read_some(buffers, ec);
  detail::throw_error(ec);
  return s;
}

template <typename MutableBufferSequence>
std::size_t file_handle::read_some(const MutableBufferSequence& buffers,
                                   asio::error_code& ec)
{
  const asio::mutable_buffer buf = *buffers.begin();
  return detail::win_file_ops::read(handle_, buf.data(), buf.size(), ec);
}

template <typename ConstBufferSequence>
std::size_t file_handle::write_some(const ConstBufferSequence& buffers)
{
  asio::error_code ec;
  std::size_t s = write_some(buffers, ec);
  detail::throw_error(ec);
  return s;
}

template <typename ConstBufferSequence>
std::size_t file_handle::write_some(const ConstBufferSequence& buffers,
                                    asio::error_code& ec)
{
  const asio::const_buffer buf = *buffers.begin();
  return detail::win_file_ops::write(handle_, buf.data(), buf.size(), ec);
}

template <typename MutableBufferSequence>
std::size_t file_handle::read_some_at(uint64_t offset,
                                      const MutableBufferSequence& buffers)
{
  asio::error_code ec;
  std::size_t s = read_some_at(offset, buffers, ec);
  detail::throw_error(ec);
  return s;
}

template <typename MutableBufferSequence>
std::size_t file_handle::read_some_at(uint64_t offset,
                                      const MutableBufferSequence& buffers,
                                      asio::error_code& ec)
{
  const asio::mutable_buffer buf = *buffers.begin();
  return detail::win_file_ops::pread(handle_, buf.data(), buf.size(),
                                     offset, ec);
}

template <typename ConstBufferSequence>
std::size_t file_handle::write_some_at(uint64_t offset,
                                       const ConstBufferSequence& buffers)
{
  asio::error_code ec;
  std::size_t s = write_some_at(offset, buffers, ec);
  detail::throw_error(ec);
  return s;
}

template <typename ConstBufferSequence>
std::size_t file_handle::write_some_at(uint64_t offset,
                                       const ConstBufferSequence& buffers,
                                       asio::error_code& ec)
{
  const asio::const_buffer buf = *buffers.begin();
  return detail::win_file_ops::pwrite(handle_, buf.data(), buf.size(),
                                      offset, ec);
}

ASIOEXT_NS_END

#include "asioext/detail/pop_options.hpp"

#endif
