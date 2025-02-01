/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_DETAIL_THROWERROR_HPP
#define ASIOEXT_DETAIL_THROWERROR_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include "asioext/error_code.hpp"

ASIOEXT_NS_BEGIN

namespace detail {

ASIOEXT_DECL void do_throw_error(const error_code& ec);
ASIOEXT_DECL void do_throw_error(const error_code& ec, const char* location);

inline void throw_error(const error_code& ec)
{
  if (ec)
    do_throw_error(ec);
}

inline void throw_error(const error_code& ec, const char* location)
{
  if (ec)
    do_throw_error(ec, location);
}

}

ASIOEXT_NS_END

#if defined(ASIOEXT_HEADER_ONLY)
# include "asioext/detail/impl/throw_error.cpp"
#endif

#endif
