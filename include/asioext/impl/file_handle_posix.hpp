/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_IMPL_FILEHANDLE_WIN_HPP
#define ASIOEXT_IMPL_FILEHANDLE_WIN_HPP

#include "asioext/file_handle.hpp"

#include "asioext/detail/posix_file_ops.hpp"
#include "asioext/detail/buffer_sequence_adapter.hpp"

#include <asio/buffer.hpp>

ASIOEXT_NS_BEGIN

template <typename MutableBufferSequence>
std::size_t file_handle::read_some(const MutableBufferSequence& buffers,
                                   asio::error_code& ec)
{
  detail::buffer_sequence_adapter<asio::mutable_buffer,
                                  MutableBufferSequence> bufs(buffers);

  return detail::posix_file_ops::readv(handle_,
                                       bufs.buffers(), bufs.count(), ec);
}

template <typename ConstBufferSequence>
std::size_t file_handle::write_some(const ConstBufferSequence& buffers,
                                    asio::error_code& ec)
{
  detail::buffer_sequence_adapter<asio::const_buffer,
                                  ConstBufferSequence> bufs(buffers);

  return detail::posix_file_ops::writev(handle_,
                                        bufs.buffers(), bufs.count(), ec);
}

template <typename MutableBufferSequence>
std::size_t file_handle::read_some_at(uint64_t offset,
                                      const MutableBufferSequence& buffers,
                                      asio::error_code& ec)
{
  detail::buffer_sequence_adapter<asio::mutable_buffer,
                                  MutableBufferSequence> bufs(buffers);

  return detail::posix_file_ops::preadv(handle_,
                                        bufs.buffers(), bufs.count(),
                                        offset, ec);
}

template <typename ConstBufferSequence>
std::size_t file_handle::write_some_at(uint64_t offset,
                                       const ConstBufferSequence& buffers,
                                       asio::error_code& ec)
{
  detail::buffer_sequence_adapter<asio::const_buffer,
                                  ConstBufferSequence> bufs(buffers);

  return detail::posix_file_ops::pwritev(handle_,
                                         bufs.buffers(), bufs.count(),
                                         offset, ec);
}

ASIOEXT_NS_END

#endif
