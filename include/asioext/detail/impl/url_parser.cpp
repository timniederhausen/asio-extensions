/// @copyright Copyright (c) 2019 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#include "asioext/error.hpp"

#include "asioext/detail/url_parser.hpp"

ASIOEXT_NS_BEGIN

namespace detail {

static constexpr int max_port_digits = 5;

ASIOEXT_CONSTEXPR14 bool isalpha_ascii(unsigned char ch) ASIOEXT_NOEXCEPT
{
  ch |= 32; // convert to lowercase
  return ch <= 'z' && ch >= 'a';
}

ASIOEXT_CONSTEXPR14 bool isalnum_ascii(unsigned char ch) ASIOEXT_NOEXCEPT
{
  return isalpha_ascii(ch) || ('0' <= ch && ch <= '9');
}

ASIOEXT_CONSTEXPR14 bool isspace_ascii(unsigned char ch) ASIOEXT_NOEXCEPT
{
  switch (ch) {
    case ' ':
    case '\f': // form-feed
    case '\n': // newline
    case '\r': // carriage return
    case '\t': // horizontal tab
    case '\v': // vertical tab
      return true;
  }
  return false;
}

ASIOEXT_CONSTEXPR14 bool is_mark(unsigned char c) ASIOEXT_NOEXCEPT
{
  switch (c) {
    case '-':
    case '_':
    case '.':
    case '!':
    case '~':
    case '*':
    case '\'':
    case '(':
    case ')':
      return true;
    default:
      return false;
  }
}

ASIOEXT_CONSTEXPR14 bool is_user_char(unsigned char c) ASIOEXT_NOEXCEPT
{
  switch (c) {
    case '%':
    case ';':
    case ':':
    case '&':
    case '=':
    case '+':
    case '$':
    case ',':
      return true;
    default:
      return isalnum_ascii(c) || is_mark(c);
  }
}

ASIOEXT_CONSTEXPR14 bool is_hex(unsigned char ch) ASIOEXT_NOEXCEPT
{
  const auto ch_lower = ch | 32; // convert to lowercase
  return (ch >= '0' && ch <= '9') || (ch_lower >= 'a' && ch_lower <= 'f');
}

ASIOEXT_CONSTEXPR14 bool is_url_char(unsigned char ch)
{
  switch (ch) {
    case 9:
    case 12:
    case '!': // 33
    case '"': // 34
    case '$': // 36
    case '%': // 37
    case '&': // 38
    case '\'': // 39
    case '(': // 40
    case ')': // 41
    case '*': // 42
    case '+': // 43
    case ',': // 44
    case '-': // 45
    case '.': // 46
    case '/': // 47
    case '0': // 48
    case '1': // 49
    case '2': // 50
    case '3': // 51
    case '4': // 52
    case '5': // 53
    case '6': // 54
    case '7': // 55
    case '8': // 56
    case '9': // 57
    case ':': // 58
    case ';': // 59
    case '<': // 60
    case '=': // 61
    case '>': // 62
    case '@': // 64
    case 'A': // 65
    case 'B': // 66
    case 'C': // 67
    case 'D': // 68
    case 'E': // 69
    case 'F': // 70
    case 'G': // 71
    case 'H': // 72
    case 'I': // 73
    case 'J': // 74
    case 'K': // 75
    case 'L': // 76
    case 'M': // 77
    case 'N': // 78
    case 'O': // 79
    case 'P': // 80
    case 'Q': // 81
    case 'R': // 82
    case 'S': // 83
    case 'T': // 84
    case 'U': // 85
    case 'V': // 86
    case 'W': // 87
    case 'X': // 88
    case 'Y': // 89
    case 'Z': // 90
    case '[': // 91
    case '\\': // 92
    case ']': // 93
    case '^': // 94
    case '_': // 95
    case '`': // 96
    case 'a': // 97
    case 'b': // 98
    case 'c': // 99
    case 'd': // 100
    case 'e': // 101
    case 'f': // 102
    case 'g': // 103
    case 'h': // 104
    case 'i': // 105
    case 'j': // 106
    case 'k': // 107
    case 'l': // 108
    case 'm': // 109
    case 'n': // 110
    case 'o': // 111
    case 'p': // 112
    case 'q': // 113
    case 'r': // 114
    case 's': // 115
    case 't': // 116
    case 'u': // 117
    case 'v': // 118
    case 'w': // 119
    case 'x': // 120
    case 'y': // 121
    case 'z': // 122
    case '{': // 123
    case '|': // 124
    case '}': // 125
    case '~': // 126
      return true;
    default:
      return false;
  }
}

void parse_authority(const char* first, const char* last, bool has_userinfo,
                     parsed_url& out, error_code& ec) ASIOEXT_NOEXCEPT
{
  if (has_userinfo) {
    auto start = first;
    while (first != last && is_user_char(*first))
      ++first;

    if (first == last || *first != '@') {
      ec = make_error_code(url_error::invalid_character);
      return;
    }

    out.userinfo = std::string_view(start, first - start);
    ++first; // skip '@'
  }

  if (first == last) {
    ec = make_error_code(url_error::missing_host);
    return;
  }

  if (*first == '[') {
    auto start = first++; // include '['
    for (bool is_zone_spec = false; first != last; ++first) {
      const char ch = *first;

      if (is_zone_spec) {
        // RFC 6874 Zone ID consists of 1*(unreserved / pct-encoded)
        if (isalnum_ascii(ch) || ch == '%' || ch == '.' || ch == '-' ||
            ch == '_' || ch == '~')
          continue;
      } else {
        if (is_hex(ch) || ch == ':' || ch == '.')
          continue;
      }

      // Zone ID must start with encoded '%' -> %25
      if (ch == '%' && last - first > 3 &&
          first[1] == '2' && first[2] == '5') {
        first += 2;
        is_zone_spec = true;
        continue;
      }

      if (ch == ']') {
        ++first;
        break;
      }

      // unrecognized -> fail
      ec = make_error_code(url_error::invalid_character);
      return;
    }

    out.host = std::string_view(start, first - start);
  } else {
    auto start = first;
    for ( ; first != last; ++first) {
      const unsigned char ch = *first;
      if (isalnum_ascii(ch) || ch == '.' || ch == '-' || ch == '_')
        continue;

      if (ch == ':')
        break;

      // unrecognized -> fail
      ec = make_error_code(url_error::invalid_character);
      return;
    }

    out.host = std::string_view(start, first - start);
  }

  // Port is optional
  if (first == last)
    return;

  if (*first == ':') {
    auto start = ++first; // skip ':'

    std::uint32_t port = 0;
    for (int num_digits = 0; first != last &&
         num_digits < max_port_digits; ++num_digits) {
      const unsigned char ch = *first++;
      if (ch < '0' || ch > '9') {
        ec = make_error_code(url_error::invalid_character);
        return;
      }
      port = port * 10 + (ch - '0');
    }
    if (port > 0xffff || first != last) {
      ec = make_error_code(url_error::port_overflow);
      return;
    }
    out.port = std::string_view(start, first - start);
    out.port_value = static_cast<std::uint16_t>(port);
    return;
  }

  ec = make_error_code(url_error::invalid_character);
}

ASIOEXT_CONSTEXPR14 url_state get_state_for(url_state s,
                                            unsigned char ch) ASIOEXT_NOEXCEPT
{
  switch (s) {
    case url_state::initial:
      if (ch == '/' || ch == '*')
        return url_state::path;

      if (isalpha_ascii(ch))
        return url_state::scheme;
      break;

    // scheme states:
    // scheme = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
    case url_state::scheme:
      if (isalnum_ascii(ch) || ch == '+' || ch == '-' || ch == '.')
        return s;

      if (ch == ':')
        return url_state::scheme_suffix;
      break;

    case url_state::scheme_suffix:
      if (ch == '/')
        return url_state::scheme_suffix_slash;
      if (is_url_char(ch))
        return url_state::path;
      break;

    case url_state::scheme_suffix_slash:
      if (ch == '/')
        return url_state::authority_start;
      if (is_url_char(ch))
        return url_state::path_after_scheme;
      break;

    // server part states:
    case url_state::authority_with_at:
      if (ch == '@') {
        return url_state::invalid;
      }
      // fall through
    case url_state::authority_start:
    case url_state::authority:
      switch (ch) {
        case '/':
          return url_state::path;
        case '?':
          return url_state::query_string_start;
        case '@':
          return url_state::authority_with_at;
        case '[':
        case ']':
          return url_state::authority;
      }
      if (is_user_char(ch))
        return url_state::authority;
      break;

    case url_state::path_after_scheme:
    case url_state::path:
      if (is_url_char(ch))
        return url_state::path;

      switch (ch) {
        case '?':
          return url_state::query_string_start;
        case '#':
          return url_state::fragment_start;
      }
      break;

    case url_state::query_string_start:
    case url_state::query_string:
      if (is_url_char(ch))
        return url_state::query_string;

      switch (ch) {
        case '?':
          // allow extra '?' in query string
          return url_state::query_string;
        case '#':
          return url_state::fragment_start;
      }
      break;

    case url_state::fragment_start:
      if (is_url_char(ch) || ch == '?') {
        return url_state::fragment;
      }

      if (ch == '#')
        return s;
      break;

    case url_state::fragment:
      if (is_url_char(ch))
        return s;

      switch (ch) {
        case '?':
        case '#':
          return s;
      }

      break;

    default:
      break;
  }

  return url_state::invalid;
}

std::string_view* part_from_state(url_state s, parsed_url& out) ASIOEXT_NOEXCEPT
{
  switch (s) {
    case url_state::scheme: return &out.scheme;
    case url_state::authority: return &out.host;
    case url_state::authority_with_at: return &out.host;
    case url_state::path_after_scheme: return &out.path;
    case url_state::path: return &out.path;
    case url_state::query_string: return &out.query;
    case url_state::fragment: return &out.fragment;
    default: return nullptr;
  }
}

void parse_url(const char* first, const char* last, parsed_url& out,
               error_code& ec) ASIOEXT_NOEXCEPT
{
  bool has_userinfo = false;
  url_state s = url_state::initial;

  std::string_view* current_part = nullptr;
  const char* it = first;
  for ( ; it != last; ++it) {
    s = get_state_for(s, *it);

    switch (s) {
      case url_state::invalid:
        ec = make_error_code(url_error::invalid_character);
        return;

      case url_state::path_after_scheme:
        // This is a special state to handle '/' after 'scheme:'.
        // A '/' can mean two things in this case:
        // * prefix for the authority part (i.e. ://)
        // * start of an absolute path (/some/dirs/file)
        // If it's the latter we use this state to rewind |it| by one
        // thus including the initial slash in the path.
        --it;
        break;

      case url_state::authority_with_at:
        has_userinfo = true;
        break;

      default: break; // silence warning
    }

    const auto part = part_from_state(s, out);
    if (part != current_part) {
      if (current_part)
        *current_part = std::string_view(first, it - first);
      current_part = part;
      first = it;
    }
  }

  if (current_part)
    *current_part = std::string_view(first, it - first);

  ec = error_code();
  if (!out.host.empty()) {
    // until this point the entire authority section is stored in |host|
    parse_authority(out.host.data(), out.host.data() + out.host.size(),
                    has_userinfo, out, ec);
  }
}

}

ASIOEXT_NS_END
