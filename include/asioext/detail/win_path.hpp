/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_DETAIL_WINPATH_HPP
#define ASIOEXT_DETAIL_WINPATH_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
#pragma once
#endif

#include <asio/error_code.hpp>

#include "asioext/detail/push_options.hpp"

ASIOEXT_NS_BEGIN

namespace detail {

class win_path
{
public:
  static const std::size_t kMaxPath = 260;

  win_path(const char* s, std::size_t len,
           asio::error_code& ec) ASIOEXT_NOEXCEPT;

  const wchar_t* c_str() const ASIOEXT_NOEXCEPT
  { return buffer_; }

private:
  wchar_t buffer_[kMaxPath + 1];
};

}

ASIOEXT_NS_END

#include "asioext/detail/pop_options.hpp"

#if defined(ASIOEXT_HEADER_ONLY)
# include "asioext/detail/impl/win_path.cpp"
#endif

#endif
