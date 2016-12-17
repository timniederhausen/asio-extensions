/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_DETAIL_IMPL_WINPATH_CPP
#define ASIOEXT_DETAIL_IMPL_WINPATH_CPP

#include "asioext/detail/win_path.hpp"
#include "asioext/detail/error.hpp"

#include <windows.h>

ASIOEXT_NS_BEGIN

namespace detail {

win_path::win_path(const char* s,
                   std::size_t len,
                   error_code& ec) ASIOEXT_NOEXCEPT
{
  int new_length = ::MultiByteToWideChar(CP_UTF8, 0, s, len, buffer_, kMaxPath);
  if (new_length == 0) {
    ec.assign(::GetLastError(), asio::error::get_system_category());
    return;
  }

  buffer_[new_length] = L'\0';
}

}

ASIOEXT_NS_END

#endif
