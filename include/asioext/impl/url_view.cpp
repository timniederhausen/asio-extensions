/// @copyright Copyright (c) 2020 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#include "asioext/url_view.hpp"

#include "asioext/detail/throw_error.hpp"

ASIOEXT_NS_BEGIN

url_view::url_view(std::string_view s)
  : spec_(s)
{
  error_code ec;
  detail::parse_url(s.data(), s.data() + s.size(), url_, ec);
  detail::throw_error(ec, "url_view (from string)");
}

url_view::url_view(std::string_view s, error_code& ec)
  : spec_(s)
{
  detail::parse_url(s.data(), s.data() + s.size(), url_, ec);
  if (ec)
    spec_ = std::string_view();
}

std::string_view url_view::port() const ASIOEXT_NOEXCEPT
{
  if (!url_.port.empty())
    return url_.port;
  if (url_.scheme == "http")
    return "80";
  if (url_.scheme == "https")
    return "443";
  if (url_.scheme == "ftp")
    return "21";
  return std::string_view();
}

std::uint16_t url_view::port_value() const ASIOEXT_NOEXCEPT
{
  if (!url_.port.empty())
    return url_.port_value;
  if (url_.scheme == "http")
    return 80;
  if (url_.scheme == "https")
    return 443;
  if (url_.scheme == "ftp")
    return 21;
  return 0;
}

ASIOEXT_NS_END
