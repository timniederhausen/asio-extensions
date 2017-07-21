/// @file
/// Defines the open_flags enum which contains flags for opening files.
///
/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_OPENFLAGS_HPP
#define ASIOEXT_OPENFLAGS_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
#pragma once
#endif

#include "asioext/detail/cstdint.hpp"
#include "asioext/detail/enum.hpp"

ASIOEXT_NS_BEGIN

/// @ingroup files
/// @brief Specifies semantics for opening files.
///
/// This enum of bitmask values controls the behaviour of the @ref open.
/// @c open_flags meets the requirements
/// of [BitmaskType](http://en.cppreference.com/w/cpp/concept/BitmaskType).
///
/// open() converts these to their platform's native equivalent (if possible).
/// Flags that are only available on certain platforms are marked as such.
///
/// There are 4 categories of flags:
/// * File access flags (@c access_read, ...)
/// * File creation disposition flags (@c create_new, ...)
/// * Sharing mode flags (@c exclusive_read, ...)
/// * Special flags (@c async, ...)
///
/// File creation disposition flags are mutually exclusive.
/// Specifying more than one is an error.
enum class open_flags
{
  /// No options are set.
  none = 0,

  // access-mode flags

  /// Request read access to the file.
  access_read = 1 << 0,

  /// Request write access to the file.
  access_write = 1 << 1,

  /// Request read and write access to the file.
  ///
  /// Note that this is equivalent to:
  /// <tt>access_read | access_write</tt>
  access_read_write = access_read | access_write,

  // creation-mode flags

  /// Attempt to create a new file. Fail if the file already exists.
  create_new = 1 << 2,

  /// Always create a new file. If the file already exists, it is truncated.
  create_always = 1 << 3,

  /// Attempt to open an existing file. Fail if no such file exists.
  open_existing = 1 << 4,

  /// Always open the file. If no such file exists, it is created.
  open_always = 1 << 5,

  /// Attempt to open and truncate the file. Fail if no such file exists.
  /// This requires the access_write bit to be set.
  truncate_existing = 1 << 6,

  // sharing

  /// Request exclusive read access to the file.
  ///
  /// @note Currently only implemented on Windows.
  exclusive_read = 1 << 7,

  /// Request exclusive write access to the file.
  ///
  /// @note Currently only implemented on Windows.
  exclusive_write = 1 << 8,

  // special

#if !defined(ASIOEXT_IS_DOCUMENTATION)
  // XXX: Intentionally undocumented, Windows only.
  /// Open the file for asynchronous operations.
  ///
  /// @warning A handle created with this flag cannot be used in combination
  /// with @c file_handle's I/O functions.
  internal_async = 1 << 9,
#endif
};

ASIOEXT_ENUM_CLASS_BITMASK_OPS(open_flags)

/// @ingroup files
/// @brief Check whether a set of open flags is valid.
///
/// This function checks whether the given @c flags are valid, i.e.
/// no mutually exclusive or unsupported flags have been specified.
ASIOEXT_DECL bool is_valid(open_flags flags);

ASIOEXT_NS_END

#if defined(ASIOEXT_HEADER_ONLY)
# include "asioext/impl/open_flags.cpp"
#endif

#endif
