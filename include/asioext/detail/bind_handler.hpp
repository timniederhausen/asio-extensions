/// @copyright Copyright (c) 2018 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_DETAIL_BINDHANDLER_HPP
#define ASIOEXT_DETAIL_BINDHANDLER_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#if defined(ASIOEXT_USE_BOOST_ASIO)
# include <boost/asio/detail/bind_handler.hpp>
#else
# include <asio/detail/bind_handler.hpp>
#endif

ASIOEXT_NS_BEGIN

namespace detail {

// TODO(tim): We shouldn't depend on asio's internals.
using asio::detail::bind_handler;

}

ASIOEXT_NS_END

#endif
