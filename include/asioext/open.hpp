/// @file
/// Declares the open() family of functions.
///
/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_OPEN_HPP
#define ASIOEXT_OPEN_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
#pragma once
#endif

#include "asioext/detail/error_code.hpp"

#include "asioext/file_handle.hpp"

#if defined(ASIOEXT_HAS_BOOST_FILESYSTEM) || defined(ASIOEXT_IS_DOCUMENTATION)
# include <boost/filesystem/path.hpp>
#endif

ASIOEXT_NS_BEGIN

/// @ingroup files
/// @defgroup filenames Filenames
/// Various platforms have different ways of representing filenames.
/// On Windows you can either use ANSI or UTF-16 filenames, while on most
/// Unix systems UTF-8 is used.
///
/// By default, this library just passes the unmodified filename it receives
/// to OS APIs.
///
/// This can be changed by defining @ref ASIOEXT_WINDOWS_USE_UTF8_FILENAMES, in
/// which case AsioExt will assume that all <tt>const char*</tt> filenames
/// passed to open() are UTF-8 encoded.
/// AsioExt will then convert them to UTF-16 and call the appropriate
/// wide character version of the API instead.

#if defined(ASIOEXT_IS_DOCUMENTATION)

/// @ingroup filenames
/// @brief If set, const char* filenames are assumed to be UTF-8 on Windows.
///
/// This macro defines whether <tt>const char*</tt> filenames are encoded
/// in the system ANSI code page (if unset) or UTF-8 (if set).
#define ASIOEXT_WINDOWS_USE_UTF8_FILENAMES

#endif

/// @ingroup files
/// @defgroup open open() family of functions
/// These functions open handles to files.
///
/// @par Implementation Detail
/// On Windows, @c CreateFileA and @c CreateFileW will be used,
/// on Unix systems @c open.
///
/// @{

/// @brief Open a file and return its handle.
///
/// This function attempts to open the specified file.
/// If successful, the returned file_handle will contain the
/// handle of the opened file. Otherwise the file_handle will be empty.
///
/// @param filename The path of the file to open.
/// See @ref filenames for details.
///
/// @param flags Flags used to open the file.
/// For a detailed reference, see @ref open_flags.
///
/// @return A handle to the opened file (or an empty handle in case
/// of failure). Ownership is transferred to the caller. Handles are not
/// inherited by child processes.
///
/// @throws asio::system_error Thrown on failure.
///
/// @note Unless manual ownership management is needed,
/// @ref scoped_file_handle::open should be preferred.
///
/// @see open_flags
/// @see filenames
ASIOEXT_DECL file_handle open(const char* filename, uint32_t flags);

/// @brief Open a file and return its handle.
///
/// This function attempts to open the specified file.
/// If successful, the returned file_handle will contain the
/// handle of the opened file. Otherwise the file_handle will be empty.
///
/// @param filename The path of the file to open.
/// See @ref filenames for details.
///
/// @param flags Flags used to open the file.
/// For a detailed reference, see @ref open_flags.
///
/// @param ec Set to indicate what error occurred. If no error occurred,
/// the object is reset.
///
/// @return A handle to the opened file (or an empty handle in case
/// of failure). Ownership is transferred to the caller. Handles are not
/// inherited by child processes.
///
/// @note Unless manual ownership management is needed,
/// @ref scoped_file_handle::open should be preferred.
///
/// @see open_flags
/// @see filenames
ASIOEXT_DECL file_handle open(const char* filename, uint32_t flags,
                              error_code& ec) ASIOEXT_NOEXCEPT;

#if defined(ASIOEXT_WINDOWS)
/// @copydoc open(const char*,uint32_t)
///
/// @note Only available on Windows.
ASIOEXT_DECL file_handle open(const wchar_t* filename, uint32_t flags);

/// @copydoc open(const char*,uint32_t,error_code&)
///
/// @note Only available on Windows.
ASIOEXT_DECL file_handle open(const wchar_t* filename, uint32_t flags,
                              error_code& ec) ASIOEXT_NOEXCEPT;
#endif

#if defined(ASIOEXT_HAS_BOOST_FILESYSTEM) || defined(ASIOEXT_IS_DOCUMENTATION)
/// @copydoc open(const char*,uint32_t)
///
/// @note Only available if ASIOEXT_HAS_BOOST_FILESYSTEM is defined.
ASIOEXT_DECL file_handle open(const boost::filesystem::path& filename,
                              uint32_t flags);

/// @copydoc open(const char*,uint32_t,error_code&)
///
/// @note Only available if ASIOEXT_HAS_BOOST_FILESYSTEM is defined.
ASIOEXT_DECL file_handle open(const boost::filesystem::path& filename,
                              uint32_t flags,
                              error_code& ec) ASIOEXT_NOEXCEPT;
#endif

/// @}

ASIOEXT_NS_END

#if defined(ASIOEXT_HEADER_ONLY)
# include "asioext/impl/open.cpp"
#endif

#endif
