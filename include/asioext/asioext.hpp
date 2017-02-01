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

/// @defgroup concepts Concepts
/// Type requirements.

/// @defgroup traits Type traits
/// Type traits provided by this library.

/// @defgroup files Files
/// Classes and functions for working with files.

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
