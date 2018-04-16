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

enum class error
{
  none = 0,
  invalid_version,
  no_acceptable_auth_method,
  invalid_auth_version,
  login_failed,
  command_not_supported,
};

ASIOEXT_DECL const error_category& get_error_category() ASIOEXT_NOEXCEPT;

inline error_code make_error_code(error e) ASIOEXT_NOEXCEPT
{
  return error_code(static_cast<int>(e), get_error_category());
}

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
