/// @file
/// Declares the various asioext::read_file overloads.
///
/// @copyright Copyright (c) 2016 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_READFILE_HPP
#define ASIOEXT_READFILE_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include "asioext/is_char_container.hpp"

#include "asioext/detail/error_code.hpp"
#include "asioext/detail/type_traits.hpp"

#if defined(ASIOEXT_HAS_BOOST_FILESYSTEM) || defined(ASIOEXT_IS_DOCUMENTATION)
# include <boost/filesystem/path.hpp>
#endif

ASIOEXT_NS_BEGIN

/// @ingroup files
/// @defgroup readfile read_file() family of functions
/// Reads the entire contents of a file into memory.
///
///@{

/// Read a file into a string.
///
/// This function loads the contents of @c filename into @c c.
///
/// @param filename The path of the file to load.
///
/// @param c The container object which shall contain the file's
/// content. The container is resized to the file's size and any previous
/// data is overwritten. The container type must satisfy the
///  @ref concept-CharContainer requirements.
///
/// @throws asio::system_error Thrown on failure.
template <class Container>
#if defined(ASIOEXT_IS_DOCUMENTATION)
void
#else
typename enable_if<is_char_container<Container>::value>::type
#endif
    read_file(const char* filename, Container& c);

/// Read a file into a string.
///
/// This function loads the contents of @c filename into @c c.
///
/// @param filename The path of the file to load.
///
/// @param c The container object which shall contain the file's
/// content. The container is resized to the file's size and any previous
/// data is overwritten. The container type must satisfy the
///  @ref concept-CharContainer requirements.
///
/// @param ec Set to indicate what error occurred. If no error occurred,
/// the object is reset.
template <class Container>
#if defined(ASIOEXT_IS_DOCUMENTATION)
void
#else
typename enable_if<is_char_container<Container>::value>::type
#endif
    read_file(const char* filename, Container& c, error_code& ec);

#if defined(ASIOEXT_WINDOWS)  || defined(ASIOEXT_IS_DOCUMENTATION)
/// @copydoc read_file(const char*,Container&)
///
/// @note Only available on Windows.
template <class Container>
#if defined(ASIOEXT_IS_DOCUMENTATION)
void
#else
typename enable_if<is_char_container<Container>::value>::type
#endif
    read_file(const wchar_t* filename, Container& c);

/// @copydoc read_file(const char*,Container&,error_code&)
///
/// @note Only available on Windows.
template <class Container>
#if defined(ASIOEXT_IS_DOCUMENTATION)
void
#else
typename enable_if<is_char_container<Container>::value>::type
#endif
    read_file(const wchar_t* filename, Container& c, error_code& ec);
#endif

#if defined(ASIOEXT_HAS_BOOST_FILESYSTEM) || defined(ASIOEXT_IS_DOCUMENTATION)
/// @copydoc read_file(const char*,Container&)
template <class Container>
#if defined(ASIOEXT_IS_DOCUMENTATION)
void
#else
typename enable_if<is_char_container<Container>::value>::type
#endif
    read_file(const boost::filesystem::path& filename, Container& c);

/// @copydoc read_file(const char*,Container&,error_code&)
template <class Container>
#if defined(ASIOEXT_IS_DOCUMENTATION)
void
#else
typename enable_if<is_char_container<Container>::value>::type
#endif
    read_file(const boost::filesystem::path& filename,
              Container& c, error_code& ec);
#endif

class file_handle;

/// Read a file into a string.
///
/// This function loads the contents of @c file into @c c.
///
/// @param file The file_handle object to read from.
/// The file_handle's file pointer is expected to point at the beginning
/// of the file. Upon completion, the file pointer points at the end.
///
/// @param c The container object which shall contain the file's
/// content. The container is resized to the file's size and any previous
/// data is overwritten. The container type must satisfy the
///  @ref concept-CharContainer requirements.
///
/// @throws asio::system_error Thrown on failure.
template <class Container>
#if defined(ASIOEXT_IS_DOCUMENTATION)
void
#else
typename enable_if<is_char_container<Container>::value>::type
#endif
    read_file(file_handle file, Container& c);

/// Read a file into a string.
///
/// This function loads the contents of @c file into @c c.
///
/// @param file The file_handle object to read from.
/// The file_handle's file pointer is expected to point at the beginning
/// of the file. Upon completion, the file pointer points at the end.
///
/// @param c The container object which shall contain the file's
/// content. The container is resized to the file's size and any previous
/// data is overwritten. The container type must satisfy the
///  @ref concept-CharContainer requirements.
///
/// @param ec Set to indicate what error occurred. If no error occurred,
/// the object is reset.
template <class Container>
#if defined(ASIOEXT_IS_DOCUMENTATION)
void
#else
typename enable_if<is_char_container<Container>::value>::type
#endif
    read_file(file_handle file, Container& c, error_code& ec);

// TODO(tim): Add support for asio's dynamic buffers,
// once they are released.

/// @}

ASIOEXT_NS_END

#include "asioext/impl/read_file.hpp"

#endif
