/// @file
/// Declares the open() family of functions.
///
/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_OPENARGS_HPP
#define ASIOEXT_OPENARGS_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
#pragma once
#endif

#include "asioext/open_flags.hpp"
#include "asioext/file_attrs.hpp"
#include "asioext/file_perms.hpp"

ASIOEXT_NS_BEGIN

/// @ingroup files_handle
/// @brief Options for a `open()` call.
///
/// This class parses AsioExt's platform-independent options for opening
/// files and stores them in a platform-**dependent** manner.
///
/// Usually objects of this type are short-lived as they're only
/// temporarily instantiated when calling @ref asioext::open.
/// However, advanced users can use this class to parse AsioExt's options
/// and then modify the platform-dependent arguments that would be passed
/// to the platform's `open()` equivalent.
class open_args
{
public:
  /// @brief Default-construct an open_args object.
  ///
  /// This constructor initializes the open_args object
  /// to an invalid state. `is_valid()` will return @c false.
  ASIOEXT_CONSTEXPR open_args() ASIOEXT_NOEXCEPT;

  /// @brief Construct a new open_args object.
  ///
  /// This constructor initializes this open_args object
  /// with the given flags. If parsing of the given arguments
  /// fails, all @c open_args members are in an unspecified state
  /// and `is_valid()` will return @c false.
  ///
  /// @param flags Flags used to open the file.
  /// For a detailed reference, see @ref open_flags.
  ///
  /// @param perms Permissions for newly created files. Unused if an existing
  /// file is opened. Defaults to @ref file_perms::create_default.
  ///
  /// @param attrs Attributes for newly created files. Unused if an existing
  /// file is opened. Defaults to @ref file_attrs::none.
  open_args(open_flags flags,
            file_perms perms = file_perms::create_default,
            file_attrs attrs = file_attrs::none) ASIOEXT_NOEXCEPT;

  /// @brief Determine if this open_args object is valid.
  ///
  /// @returns @c true if this object contains only valid options,
  /// @c false otherwise.
  ASIOEXT_CONSTEXPR bool is_valid() const ASIOEXT_NOEXCEPT
  {
#if defined(ASIOEXT_WINDOWS)
    return creation_disposition_ != 0xffffffff;
#else
    return native_flags_ != -1;
#endif
  }

#if defined(ASIOEXT_WINDOWS) || defined(ASIOEXT_IS_DOCUMENTATION)
  /// @name Windows `CreateFile()` arguments
  /// @{

  ASIOEXT_CONSTEXPR uint32_t creation_disposition() const ASIOEXT_NOEXCEPT
  { return creation_disposition_; }
  ASIOEXT_CONSTEXPR14 void creation_disposition(uint32_t value) ASIOEXT_NOEXCEPT
  { creation_disposition_ = value; }

  ASIOEXT_CONSTEXPR uint32_t desired_access() const ASIOEXT_NOEXCEPT
  { return desired_access_; }
  ASIOEXT_CONSTEXPR14 void desired_access(uint32_t value) ASIOEXT_NOEXCEPT
  { desired_access_ = value; }

  ASIOEXT_CONSTEXPR uint32_t share_mode() const ASIOEXT_NOEXCEPT
  { return share_mode_; }
  ASIOEXT_CONSTEXPR14 void share_mode(uint32_t value) ASIOEXT_NOEXCEPT
  { share_mode_ = value; }

  ASIOEXT_CONSTEXPR uint32_t attrs() const ASIOEXT_NOEXCEPT
  { return attrs_; }
  ASIOEXT_CONSTEXPR14 void attrs(uint32_t value) ASIOEXT_NOEXCEPT
  { attrs_ = value; }

  ASIOEXT_CONSTEXPR uint32_t flags() const ASIOEXT_NOEXCEPT
  { return flags_; }
  ASIOEXT_CONSTEXPR14 void flags(uint32_t value) ASIOEXT_NOEXCEPT
  { flags_ = value; }

  /// @}
#endif

#if !defined(ASIOEXT_WINDOWS) || defined(ASIOEXT_IS_DOCUMENTATION)
  /// @name POSIX `open()` arguments
  /// @{
  ASIOEXT_CONSTEXPR int native_flags() const ASIOEXT_NOEXCEPT
  { return native_flags_; }
  ASIOEXT_CONSTEXPR14 void native_flags(int value) const ASIOEXT_NOEXCEPT
  { native_flags_ = value }

  ASIOEXT_CONSTEXPR uint32_t mode() const ASIOEXT_NOEXCEPT
  { return mode_; }
  ASIOEXT_CONSTEXPR14 void mode(uint32_t value) ASIOEXT_NOEXCEPT
  { mode_ = value; }

  ASIOEXT_CONSTEXPR uint32_t attrs() const ASIOEXT_NOEXCEPT
  { return attrs_; }
  ASIOEXT_CONSTEXPR14 void attrs(uint32_t value) ASIOEXT_NOEXCEPT
  { attrs_ = value; }

  /// @}
#endif

private:
#if defined(ASIOEXT_WINDOWS)
  uint32_t creation_disposition_;
  uint32_t desired_access_;
  uint32_t share_mode_;
  uint32_t attrs_;
  uint32_t flags_;
#else
  int native_flags_;
  uint32_t mode_;
  uint32_t attrs_;
#endif
};

ASIOEXT_NS_END

#include "asioext/impl/open_args.hpp"

#endif
