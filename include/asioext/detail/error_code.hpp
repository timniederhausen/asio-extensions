/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_DETAIL_ERRORCODE_HPP
#define ASIOEXT_DETAIL_ERRORCODE_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#if defined(ASIOEXT_USE_BOOST_ASIO)
# include <boost/system/error_code.hpp>
#else
# include <asio/error_code.hpp>
#endif

ASIOEXT_NS_BEGIN

#if defined(ASIOEXT_USE_BOOST_ASIO)
using boost::system::error_code;
#else
using asio::error_code;
#endif

namespace detail {

// If std::error_code is unavailable, Asio uses a bundled custom
// error_code/error_category class, whose interface isn't conforming.

inline void clear_error(error_code& ec)
{
#if defined(ASIOEXT_USE_BOOST_ASIO) || defined(ASIO_HAS_STD_SYSTEM_ERROR)
  ec.clear();
#else
  ec = error_code();
#endif
}

}

ASIOEXT_NS_END

#endif
