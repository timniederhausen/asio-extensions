/// @copyright Copyright (c) 2017 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_DETAIL_MEMORY_HPP
#define ASIOEXT_DETAIL_MEMORY_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#if defined(ASIOEXT_HAS_STD_SHARED_PTR)
# include <memory>
#else
# include <boost/shared_ptr.hpp>
# include <boost/weak_ptr.hpp>
#endif

ASIOEXT_NS_BEGIN

namespace detail {

#if defined(ASIOEXT_HAS_STD_SHARED_PTR)
using std::shared_ptr;
using std::weak_ptr;
#else
using boost::shared_ptr;
using boost::weak_ptr;
#endif

}

ASIOEXT_NS_END

#endif
