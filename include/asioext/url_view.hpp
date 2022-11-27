/// @file
/// Defines the asioext::url_view class.
///
/// @copyright Copyright (c) 2020 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_URLVIEW_HPP
#define ASIOEXT_URLVIEW_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include "asioext/detail/url_parser.hpp"

ASIOEXT_NS_BEGIN

/// @brief Provide structured access to URL components.
///
/// This class parses an URL string into the URL's components
/// and provides access to them. No memory is allocated in the process.
class url_view
{
public:
#if defined(ASIOEXT_HAS_DEFAULTED_FUNCTIONS)
  ASIOEXT_CONSTEXPR14 url_view() ASIOEXT_NOEXCEPT = default;
#endif

  ASIOEXT_DECL url_view(std::string_view s);
  ASIOEXT_DECL url_view(std::string_view s, error_code& ec);

  explicit operator bool() const ASIOEXT_NOEXCEPT { return !spec_.empty(); }
  bool is_valid() const ASIOEXT_NOEXCEPT { return !spec_.empty(); }

  std::string_view scheme() const ASIOEXT_NOEXCEPT { return url_.scheme; }
  std::string_view userinfo() const ASIOEXT_NOEXCEPT { return url_.userinfo; }
  std::string_view host() const ASIOEXT_NOEXCEPT { return url_.host; }
  ASIOEXT_DECL std::string_view port() const ASIOEXT_NOEXCEPT;
  ASIOEXT_DECL std::uint16_t port_value() const ASIOEXT_NOEXCEPT;
  std::string_view path() const ASIOEXT_NOEXCEPT { return url_.path; }
  std::string_view query() const ASIOEXT_NOEXCEPT { return url_.query; }
  std::string_view fragment() const ASIOEXT_NOEXCEPT { return url_.fragment; }

private:
  std::string_view spec_;
  detail::parsed_url url_;
};

ASIOEXT_NS_END

#endif
