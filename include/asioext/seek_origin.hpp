/// @file
/// Defines the seek_origin enum
///
/// @copyright Copyright (c) 2017 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_SEEKORIGIN_HPP
#define ASIOEXT_SEEKORIGIN_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

ASIOEXT_NS_BEGIN

/// @ingroup files
/// Describes the various position offset origins.
///
/// This enum describes the various origins,
/// relative to which a position offset might be interpreted.
enum class seek_origin
{
  /// Interpret the offset as relative to the file's beginning
  /// (i.e. absolute)
  from_begin,

  /// Interpret the offset as relative to the current offset
  from_current,

  /// Interpret the offset as relative to the file's size.
  from_end,
};

ASIOEXT_NS_END

#endif
