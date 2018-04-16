/// @copyright Copyright (c) 2017 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_DETAIL_CHRONO_HPP
#define ASIOEXT_DETAIL_CHRONO_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#if defined(ASIOEXT_USE_BOOST_ASIO)
# include <boost/asio/coroutine.hpp>
#else
# include <asio/coroutine.hpp>
#endif

#if defined(ASIOEXT_USE_BOOST_ASIO)
# define ASIOEXT_CORO_REENTER BOOST_ASIO_CORO_REENTER
# define ASIOEXT_CORO_YIELD BOOST_ASIO_CORO_YIELD
# define ASIOEXT_CORO_FORK BOOST_ASIO_CORO_FORK
#else
# define ASIOEXT_CORO_REENTER ASIO_CORO_REENTER
# define ASIOEXT_CORO_YIELD ASIO_CORO_YIELD
# define ASIOEXT_CORO_FORK ASIO_CORO_FORK
#endif

#endif
