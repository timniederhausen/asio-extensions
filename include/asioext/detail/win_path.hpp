/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_DETAIL_WINPATH_HPP
#define ASIOEXT_DETAIL_WINPATH_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include "asioext/error_code.hpp"

ASIOEXT_NS_BEGIN

namespace detail {

// See: https://msdn.microsoft.com/en-us/library/windows/desktop/aa365247(v=vs.85).aspx
class win_path
{
public:
  static const std::size_t kMaxPath = 260;

  ASIOEXT_DECL win_path(const char* s, std::size_t len,
                        error_code& ec) ASIOEXT_NOEXCEPT;
  ASIOEXT_DECL ~win_path();

  const wchar_t* c_str() const ASIOEXT_NOEXCEPT
  {
    return heap_memory_ ? heap_memory_ : buffer_;
  }

private:
  wchar_t buffer_[kMaxPath + 1];
  wchar_t* heap_memory_;
};

}

ASIOEXT_NS_END

#if defined(ASIOEXT_HEADER_ONLY)
# include "asioext/detail/impl/win_path.cpp"
#endif

#endif
