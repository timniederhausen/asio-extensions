/// @copyright Copyright (c) 2018 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#include "asioext/socks/error.hpp"

ASIOEXT_NS_BEGIN

namespace socks {

class error_category_impl : public error_category
{
public:
  const char* name() const ASIOEXT_NOEXCEPT
  {
    return "asioext.socks";
  }

  std::string message(int value) const
  {
    switch (static_cast<error>(value)) {
      case error::none: return "none";
      case error::invalid_version: return "invalid_version";
      case error::no_acceptable_auth_method: return "no_acceptable_auth_method";
      case error::invalid_auth_version: return "invalid_auth_version";
      case error::login_failed: return "login_failed";
      case error::command_not_supported: return "command_not_supported";
    }
    return "unknown";
  }
};

const error_category& get_error_category() ASIOEXT_NOEXCEPT
{
  static const error_category_impl cat;
  return cat;
}

}

ASIOEXT_NS_END
