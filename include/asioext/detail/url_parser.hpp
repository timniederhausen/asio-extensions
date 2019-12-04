/// @copyright Copyright (c) 2019 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_DETAIL_URLPARSER_HPP
#define ASIOEXT_DETAIL_URLPARSER_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include "asioext/error_code.hpp"

#include <string_view>
#include <cstdint>

ASIOEXT_NS_BEGIN

namespace detail {

enum class url_state
{
  invalid,
  initial,

  scheme,
  scheme_suffix,
  scheme_suffix_slash,

  authority_start,
  authority,
  authority_with_at,

  path_after_scheme,
  path,

  query_string_start,
  query_string,

  fragment_start,
  fragment,
};

struct parsed_url
{
  std::string_view scheme;
  std::string_view userinfo;
  std::string_view host;
  std::string_view port;
  std::string_view path;
  std::string_view query;
  std::string_view fragment;
  std::uint16_t port_value = 0;
};

// Standalone parser for the authority part of an URL
// (splits authority into userinfo, host and port parts)
ASIOEXT_DECL void parse_authority(const char* first, const char* last,
                                  bool has_userinfo, parsed_url& out,
                                  error_code& ec) ASIOEXT_NOEXCEPT;

// helper methods
ASIOEXT_DECL ASIOEXT_CONSTEXPR14 url_state get_state_for(
    url_state s, unsigned char ch) ASIOEXT_NOEXCEPT;
ASIOEXT_DECL std::string_view* part_from_state(
    url_state s, parsed_url& out) ASIOEXT_NOEXCEPT;

// parser entry point
ASIOEXT_DECL void parse_url(const char* first, const char* last,
                            parsed_url& out, error_code& ec) ASIOEXT_NOEXCEPT;

}

ASIOEXT_NS_END

#if defined(ASIOEXT_HEADER_ONLY)
# include "asioext/detail/impl/url_parser.cpp"
#endif

#endif
