/// @file
/// Declares the duplicate() family of functions.
///
/// @copyright Copyright (c) 2016 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_DUPLICATE_HPP
#define ASIOEXT_DUPLICATE_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include "asioext/unique_file_handle.hpp"
#include "asioext/error_code.hpp"

ASIOEXT_NS_BEGIN

/// @ingroup files
/// @defgroup duplicate duplicate() family of functions
/// Duplicate a file handle, resulting in a new native handle referring to
/// the same file.
///
/// @{

/// @brief Duplicate the given file_handle's native handle.
///
/// This function duplicates the native handle and returns a
/// unique_file_handle object wrapping it.
///
/// @param handle The file_handle containing the native handle to duplicate.
///
/// @return A new unique_file_handle referring to the same file as this
/// unique_file_handle.
///
/// @throws asio::system_error Thrown on failure.
///
/// @note This is provided as a function instead of a
/// copy-contructor/assignment-operator since copying a file_handle is a
/// non-trivial operation which is rarely desired.
ASIOEXT_DECL unique_file_handle duplicate(file_handle handle);

/// @brief Duplicate the given file_handle's native handle.
///
/// This function duplicates the native handle and returns a
/// unique_file_handle object wrapping it.
///
/// @param handle The file_handle containing the native handle to duplicate.
///
/// @param ec Set to indicate what error occurred. If no error occurred,
/// the object is reset.
///
/// @return A new unique_file_handle referring to the same file as @c handle.
///
/// @note This is provided as a function instead of a
/// copy-contructor/assignment-operator since copying a file_handle is a
/// non-trivial operation which is rarely desired.
ASIOEXT_DECL unique_file_handle duplicate(file_handle handle,
                                          error_code& ec) ASIOEXT_NOEXCEPT;

/// @}

ASIOEXT_NS_END

#if defined(ASIOEXT_HEADER_ONLY)
# include "asioext/impl/duplicate.cpp"
#endif

#endif
