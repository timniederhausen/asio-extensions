/// @copyright Copyright (c) 2019 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#include "asioext/error.hpp"

ASIOEXT_NS_BEGIN

const char* url_error_category_impl::name() const ASIOEXT_NOEXCEPT
{
  return "asioext";
}

std::string url_error_category_impl::message(int value) const
{
  switch (static_cast<url_error>(value)) {
    case url_error::none: return "none";
    case url_error::invalid_character: return "invalid_character";
    case url_error::missing_host: return "missing_host";
    case url_error::port_overflow: return "port_overflow";
  }
  return "unknown";
}

ASIOEXT_DEFINE_ERRORCATEGORY(url_error_category_impl, url_error_category)

ASIOEXT_NS_END
