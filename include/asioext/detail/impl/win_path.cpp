/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "asioext/detail/win_path.hpp"
#include "asioext/detail/error.hpp"

#include <windows.h>

ASIOEXT_NS_BEGIN

namespace detail {

win_path::win_path(const char* s, std::size_t len, error_code& ec)
  : heap_memory_(nullptr)
{
  int new_length;
  if (len >= kMaxPath) {
    new_length = ::MultiByteToWideChar(CP_UTF8, 0, s, len, nullptr, 0);
    heap_memory_ = new wchar_t[new_length + 1];
    new_length = ::MultiByteToWideChar(CP_UTF8, 0, s, len,
                                       heap_memory_, new_length);
    heap_memory_[new_length] = L'\0';
  } else {
    new_length = ::MultiByteToWideChar(CP_UTF8, 0, s, len, buffer_, kMaxPath);
    buffer_[new_length] = L'\0';
  }

  if (new_length != 0) {
    ec = error_code();
  } else {
    ec.assign(::GetLastError(), asio::error::get_system_category());
  }
}

win_path::~win_path()
{
  delete[] heap_memory_;
}

}

ASIOEXT_NS_END
