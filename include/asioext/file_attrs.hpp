/// @file
/// Defines the file_attributes enum.
///
/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_FILEATTRS_HPP
#define ASIOEXT_FILEATTRS_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
#pragma once
#endif

#include "asioext/detail/cstdint.hpp"
#include "asioext/detail/enum.hpp"

ASIOEXT_NS_BEGIN

/// @ingroup files_meta
/// @{

/// @brief Names for file attributes.
///
/// This enum represents file attributes. @c file_attrs meets the requirements
/// of [BitmaskType](http://en.cppreference.com/w/cpp/concept/BitmaskType).
enum class file_attrs : uint32_t
{
  /// The file has no attributes.
  none = 0,

  /// The file is hidden. It is typically not included in directory listings.
  ///
  /// @note Only supported on Windows and FreeBSD.
  hidden = 1 << 0,

  /// The file is used (exclusively) by the operating system.
  ///
  /// @note Only supported on Windows and FreeBSD.
  system = 1 << 1,

  /// The file needs to be archived.
  ///
  /// Typically this flag is used to mark files for backup.
  ///
  /// @note Only supported on Windows and FreeBSD.
  archive = 1 << 2,

  /// The file is not backed up by the @c dump program.
  ///
  /// @note Only supported on mac OS / *BSD systems.
  no_dump = 1 << 3,

  /// The file isn't indexed by the Windows Content Indexing service.
  ///
  /// @note Only supported on Windows.
  not_indexed = 1 << 4,

  /// The file cannot be changed.
  ///
  /// @note Only supported on mac OS / *BSD systems.
  user_immutable = 1 << 5,

  /// The file cannot be changed.
  ///
  /// This attribute may only be set by the super-user.
  ///
  /// @note Only supported on mac OS / *BSD systems.
  system_immutable = 1 << 6,

  /// The file cannot be deleted, nor renamed.
  ///
  /// @note Only supported on FreeBSD.
  user_no_unlink = 1 << 7,

  /// The file cannot be deleted, nor renamed.
  ///
  /// This attribute may only be set by the super-user.
  ///
  /// @note Only supported on FreeBSD.
  system_no_unlink = 1 << 8,

  // reserved for the future...
  // --------------------------
};

ASIOEXT_ENUM_CLASS_BITMASK_OPS(file_attrs)

/// @brief Options to control the behavior of
/// @ref asioext::file_handle::attributes(file_attrs,file_attr_options)
///
/// This enum represents options for the file attributes modifier.
/// @c file_attr_options meets the requirements
/// of [BitmaskType](http://en.cppreference.com/w/cpp/concept/BitmaskType).
enum class file_attr_options
{
  /// Replace all current attributes.
  replace = 1 << 0,

  /// Replace attributes with the bitwise OR
  /// of the argument and the current attributes.
  add = 1 << 1,

  /// Replace attributes with the bitwise AND
  /// of the negated argument and the current attributes.
  remove = 1 << 2,
};

ASIOEXT_ENUM_CLASS_BITMASK_OPS(file_attr_options)

/// @}

ASIOEXT_NS_END

#endif
