/// @file
/// Defines SOCKS error codes.
///
/// @copyright Copyright (c) 2018 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_SOCKS_ERROR_HPP
#define ASIOEXT_SOCKS_ERROR_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include "asioext/error_code.hpp"

ASIOEXT_NS_BEGIN

namespace socks {

/// @ingroup net_socks
/// @defgroup net_socks_error Error handling
/// @{

/// @brief SOCKS-specific error codes
enum class error
{
  /// @brief No error.
  none = 0,

  /// @brief SOCKS version mismatch between server and client.
  ///
  /// Used in case the client receives a packet with a different
  // server major version.
  invalid_version,

  /// @brief Server supports none of our authentication methods.
  ///
  /// Used in case the server rejects all of our proposed authentication
  /// methods
  ///
  /// @see async_greet
  no_acceptable_auth_method,

  /// @brief Authentication type version mismatch between client and server.
  ///
  /// The version of the agreed-on authentication scheme is different on
  /// the server.
  invalid_auth_version,

  /// @brief The server rejected our login attempt.
  login_failed,

  /// @brief The SOCKS @c command we sent was rejected.
  ///
  /// The server doesn't understand the @c command we sent.
  command_not_supported,

  /// @brief The client's identd is not reachable from the server.
  identd_not_reachable,

  /// @brief A generic error occurred.
  ///
  /// This value is used when the SOCKS server gives us no additional
  /// information.
  generic,
};

/// @brief Get the @c error_category for @c error
ASIOEXT_DECL const error_category& get_error_category() ASIOEXT_NOEXCEPT;

inline error_code make_error_code(error e) ASIOEXT_NOEXCEPT
{
  return error_code(static_cast<int>(e), get_error_category());
}

/// @}

}

ASIOEXT_NS_END

#if defined(ASIOEXT_USE_BOOST_ASIO)
namespace boost {
namespace system {

template <>
struct is_error_code_enum<asioext::socks::error>
{
  static const bool value = true;
};

}
}
#else
namespace std {

template <>
struct is_error_code_enum<asioext::socks::error>
{
  static const bool value = true;
};

}
#endif

#if defined(ASIOEXT_HEADER_ONLY)
# include "asioext/socks/impl/error.cpp"
#endif

#endif
