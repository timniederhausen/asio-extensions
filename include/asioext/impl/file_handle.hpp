/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_IMPL_FILEHANDLE_HPP
#define ASIOEXT_IMPL_FILEHANDLE_HPP

#include "asioext/file_handle.hpp"

#include "asioext/detail/buffer.hpp"
#include "asioext/detail/throw_error.hpp"

ASIOEXT_NS_BEGIN

template <typename MutableBufferSequence>
std::size_t file_handle::read_some(const MutableBufferSequence& buffers)
{
  error_code ec;
  std::size_t s = read_some(buffers, ec);
  detail::throw_error(ec);
  return s;
}

template <typename ConstBufferSequence>
std::size_t file_handle::write_some(const ConstBufferSequence& buffers)
{
  error_code ec;
  std::size_t s = write_some(buffers, ec);
  detail::throw_error(ec);
  return s;
}

template <typename MutableBufferSequence>
std::size_t file_handle::read_some_at(uint64_t offset,
                                      const MutableBufferSequence& buffers)
{
  error_code ec;
  std::size_t s = read_some_at(offset, buffers, ec);
  detail::throw_error(ec);
  return s;
}

template <typename ConstBufferSequence>
std::size_t file_handle::write_some_at(uint64_t offset,
                                       const ConstBufferSequence& buffers)
{
  error_code ec;
  std::size_t s = write_some_at(offset, buffers, ec);
  detail::throw_error(ec);
  return s;
}

ASIOEXT_NS_END

#endif
