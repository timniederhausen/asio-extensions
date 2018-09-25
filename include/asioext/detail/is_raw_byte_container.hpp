/// @file
/// Defines the asioext::is_raw_byte_container trait.
///
/// @copyright Copyright (c) 2018 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_DETAIL_ISRAWBYTECONTAINER_HPP
#define ASIOEXT_DETAIL_ISRAWBYTECONTAINER_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include <type_traits>

ASIOEXT_NS_BEGIN

template <class T>
struct is_raw_byte_container : std::false_type
{};

ASIOEXT_NS_END

#endif
