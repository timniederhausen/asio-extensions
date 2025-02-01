/// @copyright Copyright (c) 2017 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_DETAIL_THROWEXCEPTION_HPP
#define ASIOEXT_DETAIL_THROWEXCEPTION_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

ASIOEXT_NS_BEGIN

namespace detail {

// Declare the throw_exception function for all targets.
template <typename Exception>
void throw_exception(const Exception& e);

#if !defined(ASIOEXT_NO_EXCEPTIONS)
template <typename Exception>
void throw_exception(const Exception& e)
{
  throw e;
}
#endif

}

ASIOEXT_NS_END

#endif
