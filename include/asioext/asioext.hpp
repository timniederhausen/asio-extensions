/// @file
/// Main header file which includes all public headers of this library.
/// Suitable for precompiling.
///
/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_ASIOEXT_HPP
#define ASIOEXT_ASIOEXT_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
#pragma once
#endif

/// @defgroup concepts Type requirements
/// @brief Type requirements and traits.
///
/// Type requirements and associated traits provided by this library.

/// @defgroup files Filesystem
/// @brief Provide access to file data and metadata.
///
/// This module contains classes and non-member functions that deal with
/// filesystem operations. Specifically, it aims to:
///
/// * Allow access to file data in a manner compatible with Asio and the Networking TS.
///   * This includes support for Asio's `*Stream` type requirements
///     (*SyncReadStream*, *SyncRandomAccessReadDevice*, ...)
///   * as well as an asynchronous I/O interface for files (@ref asioext::basic_file),
///     with several underlying implementations:
///     * @ref asioext::thread_pool_file_service : Blocking I/O operations are performed on the
///       thread-pool.
/// * Provide accessors and modifiers for file metadata, including:
///   * File size
///   * File permissions
///   * File attributes
///   * File time info (ctime, mtime, ...)
/// * Utilities for reading/writing files:
///   * @ref asioext::read_file
///   * @ref asioext::write_file

/// @ingroup files
/// @defgroup files_handle File handles

/// @ingroup files
/// @defgroup files_meta File metadata

/// @ingroup files
/// @defgroup files_time File times

/// @defgroup service Service utilities
/// Utility classes and functions for the implementation of
/// custom I/O objects and/or services.
///
/// @note Unless you're writing a custom service, there is probably
/// little of interest in this group.

/// AsioExt's main namespace.
namespace asioext {
}

#include "asioext/file_handle.hpp"
#include "asioext/open_flags.hpp"

#endif
