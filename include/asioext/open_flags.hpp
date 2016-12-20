/// @file
/// Defines the open_flags namespace which contains flags for opening files.
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

ASIOEXT_NS_BEGIN

/// @ingroup files
/// @defgroup openflags open() flags
/// Options for opening files.
/// open() converts these to their platform's native equivalent (if possible).
/// Flags that are only available on certain platforms are marked as such.

/// @ingroup openflags
/// @brief Contains option flags for opening files.
///
/// This namespace contains various flags for opening files.
namespace open_flags {

/// @ingroup openflags
/// @{

/// File access mode flags.
enum access_mode
{
  /// Request read access to the file.
  access_read = 1 << 0,

  /// Request write access to the file.
  access_write = 1 << 1,

  /// Request read and write access to the file.
  ///
  /// Note that this is equivalent to:
  /// <tt>access_read | access_write</tt>
  access_rw = access_read | access_write,
};

/// @brief File creation-mode flags.
///
/// These flags specify the action that should be taken on a file.
///
/// @note These are mutually exclusive. Specifying more than one will
/// cause the operation to fail.
enum creation_mode
{
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
};

/// @brief File creation attributes.
///
/// These flags specify the attributes that should be set on a newly
/// created file.
enum creation_attributes
{
  /// The file is hidden.
  ///
  /// @note Only supported on Windows.
  attribute_hidden = 1 << 7,

  /// The file is executable.
  ///
  /// @note Not supported on Windows.
  attribute_executable = 1 << 8,
};

/// @brief Check whether a set of flags is valid.
///
/// This function checks whether the given @c flags are valid, i.e.
/// no mutually exclusive or unsupported flags have been specified.
ASIOEXT_DECL bool is_valid(uint32_t flags);

/// @}

}

ASIOEXT_NS_END

#if defined(ASIOEXT_HEADER_ONLY)
# include "asioext/impl/open_flags.cpp"
#endif

#endif
