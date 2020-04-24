/// @file
/// Defines the asioext::is_raw_byte_container trait.
///
/// @copyright Copyright (c) 2016 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_ISRAWBYTECONTAINER_HPP
#define ASIOEXT_ISRAWBYTECONTAINER_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include <type_traits>

ASIOEXT_NS_BEGIN

/// @ingroup concepts
/// @defgroup concept-RawByteContainer RawByteContainer
/// A `RawByteContainer` is a special ContiguousContainer that stores POD-type
/// objects with a size of 1 byte.
///
/// ## Requirements
///
/// The type X satisfies RawByteContainer if
/// * The type X satisfies SequenceContainer, and
/// * The type X satisfies ContiguousContainer and
/// * The type @c X::value_type satisfies PODType and
/// * The type @c X::value_type has a size of 1 byte
///   (<code>sizeof(X::value_type) == 1</code>)
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
/// | `a.resize(n)` | void | erases or appends elements to meet `size() == n` | | `a.size() == n` |
///
/// @{

#if defined(ASIOEXT_IS_DOCUMENTATION)
/// @brief Determines whether T satisfies the @ref concept-RawByteContainer
/// requirements.
///
/// @note This current only verifies that `value_type` is a 1 byte POD and the
/// `data()`, `size()` and `resize()` member functions with the correct
/// signature exist.
template <typename T>
struct is_raw_byte_container
{
  /// @c true if T satisfies the @ref concept-RawByteContainer requirements,
  /// @c false otherwise.
  static const bool value;
};
#else

template <typename T, typename = void>
struct is_raw_byte_container : std::false_type
{};

template <typename T>
struct is_raw_byte_container<T, void_t<
  decltype(std::declval<T&>().data()),
  decltype(std::declval<T&>().size()),
  decltype(std::declval<T&>().resize(std::size_t())),
  std::enable_if_t<
    std::is_convertible<decltype(std::declval<T&>().data()), typename T::value_type*>::value &&
    sizeof(typename T::value_type) == 1 &&
    std::is_pod<typename T::value_type>::value
  >
>> : std::true_type
{};

#endif

/// @}

ASIOEXT_NS_END

#endif
