/// @file
/// Declares the various asioext::write_file overloads.
///
/// @copyright Copyright (c) 2016 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_WRITEFILE_HPP
#define ASIOEXT_WRITEFILE_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include "asioext/error_code.hpp"

#include "asioext/detail/asio_version.hpp"
#include "asioext/detail/buffer.hpp"

#if defined(ASIOEXT_HAS_BOOST_FILESYSTEM) || defined(ASIOEXT_IS_DOCUMENTATION)
# include <boost/filesystem/path.hpp>
#endif

ASIOEXT_NS_BEGIN

/// @ingroup files
/// @defgroup write_file asioext::write_file()
/// Writes a sequence of buffers to a file.
///
/// @par Example
/// @code
/// const std::string d1("Hallo Welt!\n");
/// const char d2[] = "Hello World!\n";
/// const std::vector<char> d3{'H', 'i', '\n'};
/// const std::array<asio::const_buffer, 3> bufs1 = {
///   asio::buffer(d1),
///   asio::buffer(d2, std::size(d2) - 1), // term. NUL
///   asio::buffer(d3)
/// };
/// asioext::write_file("test.txt", bufs1);
/// // test.txt now contains three lines: Hallo Welt!, Hello World, Hi
/// @endcode
///
///@{

#if !defined(ASIOEXT_IS_DOCUMENTATION)
# if ASIOEXT_ASIO_VERSION < 101100
// No type checking in this case...
#  define ASIOEXT_DETAIL_WRITEFILE_BUF_RET(T) void
# else
#  define ASIOEXT_DETAIL_WRITEFILE_BUF_RET(T) \
    typename std::enable_if<asio::is_const_buffer_sequence<T>::value>::type
# endif
#else
# define ASIOEXT_DETAIL_WRITEFILE_BUF_RET(T) void
#endif

/// Write a sequence of buffers to a file.
///
/// This function writes @c buffers into @c filename. If the file already
/// exists, it is overwritten. After a successful call to this function, the
/// file shall only contain the contents of the given buffers.
///
/// @param filename The path of the file into which the buffer content shall
/// be written.
///
/// @param buffers The sequence of buffers to write to the file.
///
/// @throws asio::system_error Thrown on failure.
template <class ConstBufferSequence>
ASIOEXT_DETAIL_WRITEFILE_BUF_RET(ConstBufferSequence)
    write_file(const char* filename, const ConstBufferSequence& buffers);

/// Write a sequence of buffers to a file.
///
/// This function writes @c buffers into @c filename. If the file already
/// exists, it is overwritten. After a successful call to this function, the
/// file shall only contain the contents of the given buffers.
///
/// @param filename The path of the file into which the buffer content shall
/// be written.
///
/// @param buffers The sequence of buffers to write to the file.
///
/// @param ec Set to indicate what error occurred. If no error occurred,
/// the object is reset.
template <class ConstBufferSequence>
ASIOEXT_DETAIL_WRITEFILE_BUF_RET(ConstBufferSequence)
    write_file(const char* filename, const ConstBufferSequence& buffers,
              error_code& ec) ASIOEXT_NOEXCEPT;

#if defined(ASIOEXT_WINDOWS)  || defined(ASIOEXT_IS_DOCUMENTATION)
/// @copydoc write_file(const char*,const ConstBufferSequence&)
///
/// @note Only available on Windows.
template <class ConstBufferSequence>
ASIOEXT_DETAIL_WRITEFILE_BUF_RET(ConstBufferSequence)
    write_file(const wchar_t* filename, const ConstBufferSequence& buffers);

/// @copydoc write_file(const char*,const ConstBufferSequence&,error_code&)
///
/// @note Only available on Windows.
template <class ConstBufferSequence>
ASIOEXT_DETAIL_WRITEFILE_BUF_RET(ConstBufferSequence)
    write_file(const wchar_t* filename, const ConstBufferSequence& buffers,
              error_code& ec) ASIOEXT_NOEXCEPT;
#endif

#if defined(ASIOEXT_HAS_BOOST_FILESYSTEM) || defined(ASIOEXT_IS_DOCUMENTATION)
/// @copydoc write_file(const char*,const ConstBufferSequence&)
///
/// @note Only available if using Boost.Filesystem
/// (i.e. if @c ASIOEXT_HAS_BOOST_FILESYSTEM is defined)
template <class ConstBufferSequence>
ASIOEXT_DETAIL_WRITEFILE_BUF_RET(ConstBufferSequence)
    write_file(const boost::filesystem::path& filename,
              const ConstBufferSequence& buffers);

/// @copydoc write_file(const char*,const ConstBufferSequence&,error_code&)
///
/// @note Only available if using Boost.Filesystem
/// (i.e. if @c ASIOEXT_HAS_BOOST_FILESYSTEM is defined)
template <class ConstBufferSequence>
ASIOEXT_DETAIL_WRITEFILE_BUF_RET(ConstBufferSequence)
    write_file(const boost::filesystem::path& filename,
              const ConstBufferSequence& buffers,
              error_code& ec) ASIOEXT_NOEXCEPT;
#endif

// TODO(tim): Add support for asio's dynamic buffers,
// once they are released.

/// @}

ASIOEXT_NS_END

#include "asioext/impl/write_file.hpp"

#endif
