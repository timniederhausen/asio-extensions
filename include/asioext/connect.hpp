/// @file
/// Declares the asioext::connect utility function.
///
/// @copyright Copyright (c) 2017 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_CONNECT_HPP
#define ASIOEXT_CONNECT_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include "asioext/error_code.hpp"

#include "asioext/detail/move_support.hpp"
#include "asioext/detail/async_result.hpp"
#include "asioext/detail/handler_type.hpp"

#if defined(ASIOEXT_USE_BOOST_ASIO)
# include <boost/asio/ip/tcp.hpp>
#else
# include <asio/ip/tcp.hpp>
#endif

ASIOEXT_NS_BEGIN

ASIOEXT_DECL void connect(asio::ip::tcp::socket::lowest_layer_type& socket,
                          asio::ip::tcp::resolver& resolver,
                          const asio::ip::tcp::resolver::query& q);

ASIOEXT_DECL void connect(asio::ip::tcp::socket::lowest_layer_type& socket,
                          asio::ip::tcp::resolver& resolver,
                          const asio::ip::tcp::resolver::query& q,
                          error_code& ec);

template <typename ComposedConnectHandler>
ASIOEXT_INITFN_RESULT_TYPE(ComposedConnectHandler,
                           void(error_code, asio::ip::tcp::resolver::iterator))
async_connect(asio::ip::tcp::socket::lowest_layer_type& socket,
              asio::ip::tcp::resolver& resolver,
              const asio::ip::tcp::resolver::query& q,
              ASIOEXT_MOVE_ARG(ComposedConnectHandler) handler);

ASIOEXT_NS_END

#include "asioext/impl/connect.hpp"

#if defined(ASIOEXT_HEADER_ONLY)
# include "asioext/impl/connect.cpp"
#endif

#endif
