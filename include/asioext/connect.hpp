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
#include "asioext/async_result.hpp"

#include "asioext/detail/move_support.hpp"

#if defined(ASIOEXT_USE_BOOST_ASIO)
# include <boost/asio/ip/tcp.hpp>
#else
# include <asio/ip/tcp.hpp>
#endif

ASIOEXT_NS_BEGIN

/// @ingroup net
/// @defgroup connect asioext::connect()
/// @{

/// @brief Establishes a socket connection by trying each endpoint of a
/// resolved name.
///
/// This function attempts to connect a socket by first resolving the given
/// name (using the query and resolver arguments) and then trying each
/// resolved endpoint (by calling the socket's connect member) until
/// a connection is established (or we've run out of endpoints).
///
/// @param socket The socket to be connected.
/// If the socket is already open, it will be closed.
/// @param resolver The resolver to be used for our query.
/// @param q The name to be resolved to a list of endpoints.
///
/// @throws asio::system_error Thrown on failure.
/// If the sequence is empty, the associated error_code is
/// @c asio::error::not_found.
/// Otherwise, contains the error from the last connection attempt.
ASIOEXT_DECL void connect(asio::ip::tcp::socket::lowest_layer_type& socket,
                          asio::ip::tcp::resolver& resolver,
                          const asio::ip::tcp::resolver::query& q);

/// @brief Establishes a socket connection by trying each endpoint of a name.
///
/// This function attempts to connect a socket by first resolving the given
/// name (using the query and resolver arguments) and then trying each
/// resolved endpoint (by calling the socket's connect member) until
/// a connection is established (or we've run out of endpoints).
///
/// @param socket The socket to be connected.
/// If the socket is already open, it will be closed.
/// @param resolver The resolver to be used for our query.
/// @param q The name to be resolved to a list of endpoints.
///
/// @param ec Set to indicate what error occurred, if any.
/// If the sequence is empty, set to @c asio::error::not_found.
/// Otherwise, contains the error from the last connection attempt.
ASIOEXT_DECL void connect(asio::ip::tcp::socket::lowest_layer_type& socket,
                          asio::ip::tcp::resolver& resolver,
                          const asio::ip::tcp::resolver::query& q,
                          error_code& ec);

/// @brief Asynchronously establishe a socket connection by trying each
/// endpoint of a resolved name.
///
/// This function attempts to connect a socket by first resolving the given
/// name (using the query and resolver arguments) and then trying each
/// resolved endpoint (by calling the socket's connect member) until
/// a connection is established (or we've run out of endpoints).
///
/// @param socket The socket to be connected.
/// If the socket is already open, it will be closed.
/// @param resolver The resolver to be used for our query.
/// @param q The name to be resolved to a list of endpoints.
/// @param handler The handler to be called when the connect operation
/// completes. The function signature of the handler must be:
/// @code
/// void handler(
///   // Result of operation. if the sequence is empty, set to
///   // asio::error::not_found. Otherwise, contains the
///   // error from the last connection attempt.
///   const error_code& error,

///   // On success, an iterator denoting the successfully
///   // connected endpoint. Otherwise, the end iterator.
///   Iterator iterator
/// );
/// @endcode
template <typename ComposedConnectHandler>
ASIOEXT_INITFN_RESULT_TYPE(ComposedConnectHandler,
                           void(error_code, asio::ip::tcp::resolver::iterator))
async_connect(asio::ip::tcp::socket::lowest_layer_type& socket,
              asio::ip::tcp::resolver& resolver,
              const asio::ip::tcp::resolver::query& q,
              ASIOEXT_MOVE_ARG(ComposedConnectHandler) handler);

/// @}

ASIOEXT_NS_END

#include "asioext/impl/connect.hpp"

#if defined(ASIOEXT_HEADER_ONLY)
# include "asioext/impl/connect.cpp"
#endif

#endif
