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

#include <type_traits>

#if !defined(ASIOEXT_IS_DOCUMENTATION)
# define ASIOEXT_ENUM_CLASS_BITMASK_OPS(Enum) \
inline Enum& operator&=(Enum& left, Enum right) \
{ using utype = std::underlying_type<Enum>::type; \
  left = static_cast<Enum>(static_cast<utype>(left) & \
                           static_cast<utype>(right)); \
  return (left); } \
\
inline Enum& operator|=(Enum& left, Enum right) \
{ using utype = std::underlying_type<Enum>::type; \
  left = static_cast<Enum>(static_cast<utype>(left) | \
                           static_cast<utype>(right)); \
  return (left); } \
\
inline Enum& operator^=(Enum& left, Enum right) \
{ using utype = std::underlying_type<Enum>::type; \
  left = static_cast<Enum>(static_cast<utype>(left) ^ \
                           static_cast<utype>(right)); \
  return (left); } \
\
inline constexpr Enum operator&(Enum left, Enum right) \
{ using utype = std::underlying_type<Enum>::type; \
  return static_cast<Enum>(static_cast<utype>(left) & \
                           static_cast<utype>(right)); } \
\
inline constexpr Enum operator|(Enum left, Enum right) \
{ using utype = std::underlying_type<Enum>::type; \
  return static_cast<Enum>(static_cast<utype>(left) | \
                           static_cast<utype>(right)); } \
\
inline constexpr Enum operator^(Enum left, Enum right) \
{ using utype = std::underlying_type<Enum>::type; \
  return static_cast<Enum>(static_cast<utype>(left) ^ \
                           static_cast<utype>(right)); } \
\
inline constexpr Enum operator~(Enum left) \
{ using utype = std::underlying_type<Enum>::type; \
  return static_cast<Enum>(~static_cast<utype>(left)); }
#else
#define ASIOEXT_ENUM_CLASS_BITMASK_OPS(Enum)
#endif

#endif
