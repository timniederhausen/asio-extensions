/// @file
/// Defines general error codes.
///
/// @copyright Copyright (c) 2018 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_ERROR_HPP
#define ASIOEXT_ERROR_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include "asioext/error_code.hpp"

ASIOEXT_NS_BEGIN

/// @ingroup core
/// @defgroup core_error Error handling
/// @{

/// @brief URL handling errors
enum class url_error
{
  /// @brief No error.
  none = 0,

  /// Invalid character in URL string.
  invalid_character,

  /// URL is missing the host part.
  missing_host,

  /// Port value is too high.
  port_overflow,
};

#if defined(ASIOEXT_ERRORCATEGORY_IN_HEADER)

class url_error_category_impl : public error_category
{
public:
#if defined(ASIOEXT_ERRORCATEGORY_CONSTEXPR_CTOR)
  constexpr url_error_category_impl() = default;
#endif

  ASIOEXT_DECL const char* name() const ASIOEXT_NOEXCEPT;
  ASIOEXT_DECL std::string message(int value) const;
};

#endif

/// @brief Get the @c error_category for @c error
ASIOEXT_DECLARE_ERRORCATEGORY(url_error_category_impl, url_error_category)

inline error_code make_error_code(url_error e) ASIOEXT_NOEXCEPT
{
  return error_code(static_cast<int>(e), url_error_category());
}

/// @}

ASIOEXT_NS_END

#if defined(ASIOEXT_USE_BOOST_ASIO)
namespace boost {
namespace system {

template <>
struct is_error_code_enum<asioext::url_error>
{
  static const bool value = true;
};

}
}
#else
namespace std {

template <>
struct is_error_code_enum<asioext::url_error>
{
  static const bool value = true;
};

}
#endif

#if defined(ASIOEXT_HEADER_ONLY) || defined(ASIOEXT_HAS_CONSTEXPR_ERRORCATEGORY)
# include "asioext/impl/error.cpp"
#endif

#endif
