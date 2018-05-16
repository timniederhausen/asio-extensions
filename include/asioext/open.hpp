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

#include "asioext/unique_file_handle.hpp"
#include "asioext/open_args.hpp"
#include "asioext/error_code.hpp"

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
/// By default, this library just forwards the filename it receives
/// to the OS APIs.
///
/// On Windows this can be changed by defining
/// @ref ASIOEXT_WINDOWS_USE_UTF8_FILENAMES, in which case AsioExt will assume
/// that all <tt>const char*</tt> filenames (passed to e.g. open()) are UTF-8
/// encoded. AsioExt will then convert them to UTF-16 and call the appropriate
/// wide character version of the API instead.

/// @ingroup files_handle
/// @defgroup open asioext::open()
/// @brief Open a handle to a file.
///
/// @{

/// @brief Open a file and return its handle.
///
/// This function opens the specified file and returns a handle to it.
///
/// @param filename The path of the file to open.
/// See @ref filenames for details.
///
/// @param args Additional options used to open the file.
///
/// @return A handle to the opened file. Ownership is transferred to the
/// caller. Handles are not inherited by child processes.
///
/// @throws asio::system_error Thrown on failure.
///
/// @see open_flags
/// @see filenames
ASIOEXT_DECL unique_file_handle open(const char* filename,
                                     const open_args& args);

/// @brief Open a file and return its handle.
///
/// This function attempts to open the specified file.
/// If successful, the returned file_handle will contain the
/// handle of the opened file. Otherwise the file_handle will be empty.
///
/// @param filename The path of the file to open.
/// See @ref filenames for details.
///
/// @param args Additional options used to open the file.
///
/// @param ec Set to indicate what error occurred. If no error occurred,
/// the object is reset.
///
/// @return A handle to the opened file (or an empty handle in case
/// of failure). Ownership is transferred to the caller. Handles are not
/// inherited by child processes.
///
/// @see open_flags
/// @see filenames
ASIOEXT_DECL unique_file_handle open(
    const char* filename, const open_args& args,
    error_code& ec) ASIOEXT_NOEXCEPT;

#if defined(ASIOEXT_WINDOWS) || defined(ASIOEXT_IS_DOCUMENTATION)
/// @copydoc open(const char*,const open_args&)
///
/// @note Only available on Windows.
ASIOEXT_DECL unique_file_handle open(
    const wchar_t* filename, const open_args& args);

/// @copydoc open(const char*,const open_args&,error_code&)
///
/// @note Only available on Windows.
ASIOEXT_DECL unique_file_handle open(
    const wchar_t* filename, const open_args& args,
    error_code& ec) ASIOEXT_NOEXCEPT;
#endif

#if defined(ASIOEXT_HAS_BOOST_FILESYSTEM) || defined(ASIOEXT_IS_DOCUMENTATION)
/// @copydoc open(const char*,const open_args&)
///
/// @note Only available if using Boost.Filesystem
/// (i.e. if @c ASIOEXT_HAS_BOOST_FILESYSTEM is defined)
ASIOEXT_DECL unique_file_handle open(
    const boost::filesystem::path& filename, const open_args& args);

/// @copydoc open(const char*,const open_args&,error_code&)
///
/// @note Only available if using Boost.Filesystem
/// (i.e. if @c ASIOEXT_HAS_BOOST_FILESYSTEM is defined)
ASIOEXT_DECL unique_file_handle open(
    const boost::filesystem::path& filename, const open_args& args,
    error_code& ec) ASIOEXT_NOEXCEPT;
#endif

/// @}

ASIOEXT_NS_END

#if defined(ASIOEXT_HEADER_ONLY)
# include "asioext/impl/open.cpp"
#endif

#endif
