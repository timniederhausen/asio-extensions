/// @copyright Copyright (c) 2017 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_DETAIL_ENUM_HPP
#define ASIOEXT_DETAIL_ENUM_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#define ASIOEXT_ENUM_CLASS_BITMASK_OPS(Enum) \
inline Enum& operator&=(Enum& left, Enum right) \
{ left = (Enum)((int)left & (int)right); return (left); } \
\
inline Enum& operator|=(Enum& left, Enum right) \
{ left = (Enum)((int)left | (int)right); return (left); } \
\
inline Enum& operator^=(Enum& left, Enum right) \
{ left = (Enum)((int)left ^ (int)right); return (left); } \
\
inline constexpr Enum operator&(Enum left, Enum right) \
{ return ((Enum)((int)left & (int)right)); } \
\
inline constexpr Enum operator|(Enum left, Enum right) \
{ return ((Enum)((int)left | (int)right)); } \
\
inline constexpr Enum operator^(Enum left, Enum right) \
{ return ((Enum)((int)left ^ (int)right)); } \
\
inline constexpr Enum operator~(Enum left) \
{ return ((Enum)~(int)left); }

#endif
