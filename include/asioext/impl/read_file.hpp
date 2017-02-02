/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_IMPL_READFILE_HPP
#define ASIOEXT_IMPL_READFILE_HPP

#include "asioext/file_handle.hpp"
#include "asioext/scoped_file_handle.hpp"
#include "asioext/open_flags.hpp"

#include "asioext/detail/error_code.hpp"
#include "asioext/detail/error.hpp"
#include "asioext/detail/buffer.hpp"

#if defined(ASIOEXT_USE_BOOST_ASIO)
# include <boost/asio/read.hpp>
#else
# include <asio/read.hpp>
#endif

#include <limits>

ASIOEXT_NS_BEGIN

template <class CharContainer>
typename enable_if<is_char_container<CharContainer>::value>::type
    read_file(const char* filename, CharContainer& c)
{
  error_code ec;
  read_file(filename, c, ec);
  detail::throw_error(ec);
}

template <class CharContainer>
typename enable_if<is_char_container<CharContainer>::value>::type
    read_file(const char* filename, CharContainer& c, error_code& ec)
{
  scoped_file_handle file(filename, access_read | open_existing, ec);
  if (!ec)
    read_file(file.get(), c, ec);
}

#if defined(ASIOEXT_WINDOWS)
template <class CharContainer>
typename enable_if<is_char_container<CharContainer>::value>::type
    read_file(const wchar_t* filename, CharContainer& c)
{
  error_code ec;
  read_file(filename, c, ec);
  detail::throw_error(ec);
}

template <class CharContainer>
typename enable_if<is_char_container<CharContainer>::value>::type
    read_file(const wchar_t* filename, CharContainer& c, error_code& ec)
{
  scoped_file_handle file(filename, access_read | open_existing, ec);
  if (!ec)
    read_file(file.get(), c, ec);
}
#endif

#if defined(ASIOEXT_HAS_BOOST_FILESYSTEM)
template <class CharContainer>
typename enable_if<is_char_container<CharContainer>::value>::type
    read_file(const boost::filesystem::path& filename, CharContainer& c)
{
  error_code ec;
  read_file(filename, c, ec);
  detail::throw_error(ec);
}

template <class CharContainer>
typename enable_if<is_char_container<CharContainer>::value>::type
    read_file(const boost::filesystem::path& filename,
              CharContainer& c, error_code& ec)
{
  scoped_file_handle file(filename, access_read | open_existing, ec);
  if (!ec)
    read_file(file.get(), c, ec);
}
#endif

template <class CharContainer>
typename enable_if<is_char_container<CharContainer>::value>::type
    read_file(file_handle file, CharContainer& c)
{
  error_code ec;
  read_file(file, c, ec);
  detail::throw_error(ec);
}

template <class CharContainer>
typename enable_if<is_char_container<CharContainer>::value>::type
    read_file(file_handle file, CharContainer& c, error_code& ec)
{
  const uint64_t size = file.size(ec);
  if (ec) return;

  if (size > std::numeric_limits<typename CharContainer::size_type>::max() ||
      size > c.max_size()) {
    ec = asio::error::message_size;
    return;
  }

  if (size != 0) {
    c.resize(static_cast<typename CharContainer::size_type>(size));
    asio::read(file, asio::buffer(&c[0], c.size()), ec);
  } else {
    c.clear();
  }
}

ASIOEXT_NS_END

#endif
