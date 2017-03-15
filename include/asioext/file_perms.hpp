/// @file
/// Defines the perms enum.
///
/// @copyright Copyright (c) 2017 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_FILEPERMS_HPP
#define ASIOEXT_FILEPERMS_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
#pragma once
#endif

#include "asioext/detail/cstdint.hpp"
#include "asioext/detail/enum.hpp"

ASIOEXT_NS_BEGIN

/// @ingroup files
/// @brief Names for permissions.
///
/// This type represents file access permissions.
/// @c file_perms meets the requirements
/// of [BitmaskType](http://en.cppreference.com/w/cpp/concept/BitmaskType).
enum class file_perms
{
  /// No permissions are set.
  none = 0,

  /// S_IRUSR, Read permission, owner
  owner_read = 0400,

  /// S_IWUSR, Write permission, owner
  owner_write = 0200,

  /// S_IXUSR, Execute/search permission, owner
  owner_exec = 0100,

  /// S_IRWXU, Read, write, execute/search by owner
  owner_all = 0700,

  /// S_IRGRP, Read permission, group
  group_read = 040,

  /// S_IWGRP, Write permission, group
  group_write = 020,

  /// S_IXGRP, Execute/search permission, group
  group_exec = 010,

  /// S_IRWXG, Read, write, execute/search by group
  group_all = 070,

  /// S_IROTH, Read permission, others
  others_read = 04,

  /// S_IWOTH, Write permission, others
  others_write = 02,

  /// S_IXOTH, Execute/search permission, others
  others_exec = 01,

  /// S_IRWXO, Read, write, execute/search by others
  others_all = 07,

  /// All users have read, write, and execute/search permissions.
  ///
  /// Equivalent to `owner_all | group_all | others_all`
  all = 0777,

  /// S_ISUID, Set-user-ID on execution
  set_uid = 04000,

  /// S_ISGID, Set-group-ID on execution
  set_gid = 02000,

  /// S_ISVTX,
  /// * **POSIX XSI**: On directories, restricted deletion flag
  /// * **V7**: 'sticky bit': save swapped text even after use
  /// * **SunOS**: On non-directories: don't cache this file
  /// * **SVID-v4.2**: On directories: restricted deletion flag
  ///
  /// @see http://en.wikipedia.org/wiki/Sticky_bit
  sticky_bit = 01000,

  /// Default permissions for newly created files (like @c DEFFILEMODE).
  ///
  /// Set to @c 0666 (
  /// `owner_read | owner_write | group_read | group_write |
  ///  others_read | others_write`) on all systems.
  ///
  /// @note Actual file permissions might differ, as these permissions
  /// are modified by the process' umask.
  create_default = 0666,

  /// All valid permission bits
  ///
  /// Equivalent to `all | set_uid | set_gid | sticky_bit`.
  mask = 07777,

  /// Unknown permissions
  unknown = 0xFFFF,

  /// Control bit to add, but not clear permission bits
  add_perms = 0x10000,

  /// Control bit to clear, but not add permission bits
  remove_perms = 0x20000,

  /// Control bit to not resolve symlinks
  ///
  /// @note Implied on Windows.
  resolve_symlinks = 0x4000,
};

ASIOEXT_ENUM_CLASS_BITMASK_OPS(file_perms)

/// @brief Check whether a set of perms is valid.
///
/// This function checks whether the given @c flags are valid, i.e.
/// no mutually exclusive or unsupported flags have been specified.
ASIOEXT_DECL bool is_valid(file_perms flags);

ASIOEXT_NS_END

#if defined(ASIOEXT_HEADER_ONLY)
# include "asioext/impl/file_perms.cpp"
#endif

#endif
