/// @file
/// Defines basic SOCKS 4/5 protocol constants.
///
/// @copyright Copyright (c) 2018 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_SOCKS_CONSTANTS_HPP
#define ASIOEXT_SOCKS_CONSTANTS_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include "asioext/detail/cstdint.hpp"

ASIOEXT_NS_BEGIN

namespace socks {

/// @ingroup net_socks
/// @{

/// @brief SOCKS commands
///
/// This enum contains the supported SOCKS
/// commands the client can ask the server to
/// execute.
enum class command : uint8_t
{
  /// Establish and proxy a TCP connection to the specified target host.
  connect = 1,

  /// Bind to the specified port and listen for connections.
  bind = 2,

  /// Associate an UDP port.
  bind_udp = 3,
};

/// @brief SOCKS versions
///
/// This enum contains constants representing
/// the different SOCKS versions.
enum class version : uint8_t
{
  v4,
  v5,
};

/// @brief SOCKS login authentication methods.
///
/// This enum contains the authentication methods
/// listed in the protocol specification.
enum class auth_method : uint8_t
{
  /// @brief No authentication.
  ///
  /// If sent by the client, indicates that the client
  /// supports/allows anonymous access to a SOCKS server.
  none = 0,

  /// @brief GSSAPI authentication.
  ///
  /// @note There is currently no public function that handles the
  /// login challenge for this authentication type.
  gssapi = 1,

  /// @brief Username + Password authentication.
  ///
  /// Indicates that the client wishes to authenticate the user
  /// with a username and password pair.
  ///
  /// @see async_login
  username_password = 2,

  /// @brief Server response if no acceptable method was given.
  ///
  /// This special enum value is used in case the server rejects all
  /// authentication methods it was given.
  no_acceptable = 0xff
};

/// @}

}

ASIOEXT_NS_END

#endif
