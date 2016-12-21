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

#if defined(ASIOEXT_HAS_BOOST_FILESYSTEM) || defined(ASIOEXT_IS_DOCUMENTATION)
# include <boost/filesystem/path.hpp>
#endif

#include <string>

ASIOEXT_NS_BEGIN

/// @ingroup files
/// @defgroup readfile read_file() family of functions
/// Reads the entire contents of a file into memory.
///
///@{

/// Read a file into a string.
///
/// This function loads the contents of @c filename into @c str.
///
/// @param filename The path of the file to load.
///
/// @param str The string object which shall contain the file's
/// content. The string is resized to the file's size and any previous
/// data is overwritten.
///
/// @throws asio::system_error Thrown on failure.
ASIOEXT_DECL void read_file(const char* filename, std::string& str);

/// Read a file into a string.
///
/// This function loads the contents of @c filename into @c str.
///
/// @param filename The path of the file to load.
///
/// @param str The string object which shall contain the file's
/// content. The string is resized to the file's size and any previous
/// data is overwritten.
///
/// @param ec Set to indicate what error occurred. If no error occurred,
/// the object is reset.
ASIOEXT_DECL void read_file(const char* filename,
                            std::string& str,
                            error_code& ec);

#if defined(ASIOEXT_WINDOWS)  || defined(ASIOEXT_IS_DOCUMENTATION)
/// @copydoc read_file(const char*,std::string&)
///
/// @note Only available on Windows.
ASIOEXT_DECL void read_file(const char* filename, std::string& str);

/// @copydoc read_file(const char*,std::string&,error_code&)
///
/// @note Only available on Windows.
ASIOEXT_DECL void read_file(const char* filename,
                            std::string& str,
                            error_code& ec);
#endif

#if defined(ASIOEXT_HAS_BOOST_FILESYSTEM) || defined(ASIOEXT_IS_DOCUMENTATION)
/// @copydoc read_file(const char*,std::string&)
ASIOEXT_DECL void read_file(const boost::filesystem::path& filename,
                            std::string& str);

/// @copydoc read_file(const char*,std::string&,error_code&)
ASIOEXT_DECL void read_file(const boost::filesystem::path& filename,
                            std::string& str,
                            error_code& ec);
#endif

class file_handle;

/// Read a file into a string.
///
/// This function loads the contents of @c file into str.
///
/// @param file The file_handle object to read from.
/// The file_handle's file pointer is expected to point at the beginning
/// of the file. Upon completion, the file pointer points at the end.
///
/// @param str The string object which shall contain the file's
/// content. The string is resized to the file's size and any previous
/// data is overwritten.
///
/// @throws asio::system_error Thrown on failure.
ASIOEXT_DECL void read_file(file_handle file, std::string& str);

/// Read a file into a string.
///
/// This function loads the contents of @c file into str.
///
/// @param file The file_handle object to read from.
/// The file_handle's file pointer is expected to point at the beginning
/// of the file. Upon completion, the file pointer points at the end.
///
/// @param str The string object which shall contain the file's
/// content. The string is resized to the file's size and any previous
/// data is overwritten.
///
/// @param ec Set to indicate what error occurred. If no error occurred,
/// the object is reset.
ASIOEXT_DECL void read_file(file_handle file,
                            std::string& str,
                            error_code& ec);

// TODO(tim): Add support for asio's dynamic buffers,
// once they are released.

/// @}

ASIOEXT_NS_END

#if defined(ASIOEXT_HEADER_ONLY)
# include "asioext/impl/read_file.cpp"
#endif

#endif
