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
// the global stdin, etc. which might be implemented as macros.

file_handle get_stdin();
file_handle get_stdin(error_code& ec);

file_handle get_stdout();
file_handle get_stdout(error_code& ec);

file_handle get_stderr();
file_handle get_stderr(error_code& ec);

/// @}

ASIOEXT_NS_END

#if defined(ASIOEXT_HEADER_ONLY)
# include "asioext/impl/standard_streams.cpp"
#endif

#endif
