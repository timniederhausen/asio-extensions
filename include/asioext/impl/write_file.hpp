/// @copyright Copyright (c) 2017 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_IMPL_WRITEFILE_HPP
#define ASIOEXT_IMPL_WRITEFILE_HPP

#include "asioext/file_handle.hpp"
#include "asioext/scoped_file_handle.hpp"
#include "asioext/open_flags.hpp"

#include "asioext/detail/error_code.hpp"
#include "asioext/detail/error.hpp"
#include "asioext/detail/buffer.hpp"

#if defined(ASIOEXT_USE_BOOST_ASIO)
# include <boost/asio/write.hpp>
#else
# include <asio/write.hpp>
#endif

#include <limits>

ASIOEXT_NS_BEGIN

template <class ConstBufferSequence>
ASIOEXT_DETAIL_WRITEFILE_BUF_RET(ConstBufferSequence)
    write_file(const char* filename, const ConstBufferSequence& buffers)
{
  error_code ec;
  write_file(filename, buffers, ec);
  detail::throw_error(ec, "write_file");
}

template <class ConstBufferSequence>
ASIOEXT_DETAIL_WRITEFILE_BUF_RET(ConstBufferSequence)
    write_file(const char* filename, const ConstBufferSequence& buffers,
              error_code& ec)
{
  scoped_file_handle file(filename,
                          open_flags::access_write | open_flags::create_always,
                          ec);
  if (!ec)
    asio::write(file, buffers, ec);
}

#if defined(ASIOEXT_WINDOWS)  || defined(ASIOEXT_IS_DOCUMENTATION)
template <class ConstBufferSequence>
ASIOEXT_DETAIL_WRITEFILE_BUF_RET(ConstBufferSequence)
    write_file(const wchar_t* filename, const ConstBufferSequence& buffers)
{
  error_code ec;
  write_file(filename, buffers, ec);
  detail::throw_error(ec, "write_file");
}

template <class ConstBufferSequence>
ASIOEXT_DETAIL_WRITEFILE_BUF_RET(ConstBufferSequence)
    write_file(const wchar_t* filename, const ConstBufferSequence& buffers,
              error_code& ec)
{
  scoped_file_handle file(filename,
                          open_flags::access_write | open_flags::create_always,
                          ec);
  if (!ec)
    asio::write(file, buffers, ec);
}
#endif

#if defined(ASIOEXT_HAS_BOOST_FILESYSTEM) || defined(ASIOEXT_IS_DOCUMENTATION)
template <class ConstBufferSequence>
ASIOEXT_DETAIL_WRITEFILE_BUF_RET(ConstBufferSequence)
    write_file(const boost::filesystem::path& filename,
              const ConstBufferSequence& buffers)
{
  error_code ec;
  write_file(filename, buffers, ec);
  detail::throw_error(ec, "write_file");
}

template <class ConstBufferSequence>
ASIOEXT_DETAIL_WRITEFILE_BUF_RET(ConstBufferSequence)
    write_file(const boost::filesystem::path& filename,
              const ConstBufferSequence& buffers, error_code& ec)
{
  scoped_file_handle file(filename,
                          open_flags::access_write | open_flags::create_always,
                          ec);
  if (!ec)
    asio::write(file, buffers, ec);
}
#endif

ASIOEXT_NS_END

#endif
