/// @file
/// Defines the scoped_file_handle class
///
/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_SCOPEDFILEHANDLE_HPP
#define ASIOEXT_SCOPEDFILEHANDLE_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include "asioext/unique_file_handle.hpp"

ASIOEXT_NS_BEGIN

#if !defined(ASIOEXT_NO_DEPRECATED)
/// (Deprecated) Alias for @c unique_file_handle
typedef unique_file_handle scoped_file_handle;
#endif

ASIOEXT_NS_END

#endif
