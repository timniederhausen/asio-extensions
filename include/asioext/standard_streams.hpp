/// @file
/// Declares functions for retrieving standard handles.
///
/// @copyright Copyright (c) 2016 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_STANDARDSTREAMS_HPP
#define ASIOEXT_STANDARDSTREAMS_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include "asioext/file_handle.hpp"

#include "asioext/detail/error_code.hpp"

ASIOEXT_NS_BEGIN

/// @ingroup files
/// @defgroup stdhandles Standard stream handles
/// Functions returning handles to @c stderr, @c stdout, @c stdin.
///
/// @{

// NOTE: These names are prefixed with get_ so they don't conflict with
// `stdin` etc. which might be implemented as macros.

/// @brief Get a handle to the standard input.
///
/// This function returns a handle to the standard input.
///
/// @return A handle to the standard input (or an empty handle in case
/// of failure). Ownership is *not* transferred to the caller.
/// Closing the handle returned by this function leads to undefined behavior.
///
/// @throws asio::system_error Thrown on failure.
ASIOEXT_DECL file_handle get_stdin();

/// @brief Get a handle to the standard input.
///
/// This function returns a handle to the standard input.
///
/// @param ec Set to indicate what error occurred. If no error occurred,
/// the object is reset.
///
/// @return A handle to the standard input (or an empty handle in case
/// of failure). Ownership is *not* transferred to the caller.
/// Closing the handle returned by this function leads to undefined behavior.
ASIOEXT_DECL file_handle get_stdin(error_code& ec);

/// @brief Get a handle to the standard output.
///
/// This function returns a handle to the standard output.
///
/// @return A handle to the standard output (or an empty handle in case
/// of failure). Ownership is *not* transferred to the caller.
/// Closing the handle returned by this function leads to undefined behavior.
///
/// @throws asio::system_error Thrown on failure.
ASIOEXT_DECL file_handle get_stdout();

/// @brief Get a handle to the standard output.
///
/// This function returns a handle to the standard output.
///
/// @param ec Set to indicate what error occurred. If no error occurred,
/// the object is reset.
///
/// @return A handle to the standard output (or an empty handle in case
/// of failure). Ownership is *not* transferred to the caller.
/// Closing the handle returned by this function leads to undefined behavior.
ASIOEXT_DECL file_handle get_stdout(error_code& ec);

/// @brief Get a handle to the standard error.
///
/// This function returns a handle to the standard error.
///
/// @return A handle to the standard error (or an empty handle in case
/// of failure). Ownership is *not* transferred to the caller.
/// Closing the handle returned by this function leads to undefined behavior.
///
/// @throws asio::system_error Thrown on failure.
ASIOEXT_DECL file_handle get_stderr();

/// @brief Get a handle to the standard error.
///
/// This function returns a handle to the standard error.
///
/// @param ec Set to indicate what error occurred. If no error occurred,
/// the object is reset.
///
/// @return A handle to the standard error (or an empty handle in case
/// of failure). Ownership is *not* transferred to the caller.
/// Closing the handle returned by this function leads to undefined behavior.
ASIOEXT_DECL file_handle get_stderr(error_code& ec);

/// @}

ASIOEXT_NS_END

#if defined(ASIOEXT_HEADER_ONLY)
# include "asioext/impl/standard_streams.cpp"
#endif

#endif
