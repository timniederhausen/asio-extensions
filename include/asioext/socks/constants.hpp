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

enum class command : uint8_t
{
  connect = 1,
  bind = 2,
  bind_udp = 3,
};

enum class version : uint8_t
{
  v4,
  v5,
};

enum class auth_method : uint8_t
{
  none = 0,
  gssapi = 1,
  username_password = 2,
  no_acceptable = 0xff
};

}

ASIOEXT_NS_END

#endif
