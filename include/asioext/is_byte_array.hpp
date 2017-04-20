/// @file
/// Defines the asioext::is_byte_array trait.
///
/// @copyright Copyright (c) 2016 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_ISBYTEARRAY_HPP
#define ASIOEXT_ISBYTEARRAY_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include <string>
#include <vector>
#include <type_traits>

ASIOEXT_NS_BEGIN

/// @ingroup concepts
/// @defgroup concept-ByteArray ByteArray
/// A `ByteArray` is a special ContiguousContainer that stores POD-type objects
/// with a size of 1 byte.
///
/// ## Requirements
///
/// The type X satisfies ByteArray if
/// * The type X satisfies SequenceContainer, and
/// * The type X satisfies ContiguousContainer and
/// * The type @c X::value_type satisfies PODType and
/// * The type @c X::value_type has a size of 1 byte
///   (<code>sizeof(X::value_type) == 1</code>) and
///
/// Given:
///
/// * @c a, rvalue expression of type of type X
/// * @c n, a value of type @c X::size_type
///
/// The following expressions must be valid and have their specified effects:
///
/// | expression | return type | effects | precondition | postcondition |
/// | ---------- | ----------- | ------- | ------------ | ------------- |
/// | a.resize(n) | void | erases or appends elements to meet @c size() == @c n | | @c a.size() == n |
///

/// @ingroup traits
/// @{

#if defined(ASIOEXT_IS_DOCUMENTATION)
/// Determines whether T satisfies the \ref concept-ByteArray requirements.
///
/// @note Currently there is no automatic way of determining conformance,
/// so by default @c value is false. There are partial specializations for
/// std::vector and std::basic_string that define value as @c true if the
/// contained type is one of @c char, `unsigned char`, `signed char`.
template <class T>
struct is_byte_array
{
  /// @c true if T satisfies the \ref concept-ByteArray requirements,
  /// @c false otherwise.
  static const bool value;
};
#else
template <class T>
struct is_byte_array : std::false_type
{};

template <class Traits, class Allocator>
struct is_byte_array<std::basic_string<char, Traits, Allocator> >
  : std::true_type
{};

template <class Traits, class Allocator>
struct is_byte_array<std::basic_string<unsigned char, Traits, Allocator> >
  : std::true_type
{};

template <class Traits, class Allocator>
struct is_byte_array<std::basic_string<signed char, Traits, Allocator> >
  : std::true_type
{};

template <class Allocator>
struct is_byte_array<std::vector<char, Allocator> >
  : std::true_type
{};

template <class Allocator>
struct is_byte_array<std::vector<unsigned char, Allocator> >
  : std::true_type
{};

template <class Allocator>
struct is_byte_array<std::vector<signed char, Allocator> >
  : std::true_type
{};
#endif

/// @}

ASIOEXT_NS_END

#endif
