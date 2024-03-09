/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_DETAIL_BUFFER_HPP
#define ASIOEXT_DETAIL_BUFFER_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#if defined(ASIOEXT_USE_BOOST_ASIO)
# include <boost/asio/buffer.hpp>
#else
# include <asio/buffer.hpp>
#endif

#if defined(ASIOEXT_USE_BOOST_ASIO)
# define ASIOEXT_MUTABLE_BUFFER boost::asio::BOOST_ASIO_MUTABLE_BUFFER
# define ASIOEXT_CONST_BUFFER boost::asio::BOOST_ASIO_CONST_BUFFER
#else
# define ASIOEXT_MUTABLE_BUFFER asio::ASIO_MUTABLE_BUFFER
# define ASIOEXT_CONST_BUFFER asio::ASIO_CONST_BUFFER
#endif

ASIOEXT_NS_BEGIN

// If we only can process one buffer at a time, we previously just used the
// first. On newer asio versions (1.11.0+) this fails if the first buffer has
// a length of zero, in which case we end up in an infinite loop.
template <typename MutableBufferSequence>
asio::mutable_buffer first_mutable_buffer(const MutableBufferSequence& buffers) ASIOEXT_NOEXCEPT
{
  const auto last = asio::buffer_sequence_end(buffers);
  for (auto first = asio::buffer_sequence_begin(buffers); first != last; ) {
    if (asio::buffer_size(*first) != 0)
      return *first;
    ++first;
  }
  return asio::mutable_buffer();
}

template <typename ConstBufferSequence>
asio::const_buffer first_const_buffer(const ConstBufferSequence& buffers) ASIOEXT_NOEXCEPT
{
  const auto last = asio::buffer_sequence_end(buffers);
  for (auto first = asio::buffer_sequence_begin(buffers); first != last; ) {
    if (asio::buffer_size(*first) != 0)
      return *first;
    ++first;
  }
  return asio::const_buffer();
}

ASIOEXT_NS_END

#endif
