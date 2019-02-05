/// @file
/// SOCKS client functionality.
///
/// @copyright Copyright (c) 2018 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_SOCKS_CLIENT_HPP
#define ASIOEXT_SOCKS_CLIENT_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include "asioext/socks/constants.hpp"
#include "asioext/async_result.hpp"
#include "asioext/linear_buffer.hpp"

#include "asioext/detail/move_support.hpp"

#if defined(ASIOEXT_USE_BOOST_ASIO)
# include <boost/asio/ip/tcp.hpp>
#else
# include <asio/ip/tcp.hpp>
#endif

ASIOEXT_NS_BEGIN

namespace socks {

/// @ingroup net_socks
/// @defgroup net_socks_v4a SOCKS4a functions
/// SOCKS4a client implementation
/// @{

/// @brief Asynchronously attempt to execute the given command.
///
/// This function asynchronously executes the given SOCKS command.
/// The following commands are supported by a SOCKS v4 server:
///
/// * @c connect: Establish a connection to the given remote endpoint.
/// Any data sent/received on the socket after successful command execution
/// belongs to the proxied connection.
///
/// @param socket A socket connected to the SOCKS server.
/// @param cmd The command to execute.
/// @param remote The remote endpoint. Only IPv4 endpoints are supported.
/// @param user_id An opqaue user id string that will be sent to the SOCKS server.
/// @param buffer A linear_buffer that is used to buffer sent/received messages.
/// @param handler The handler to be called when the execute operation
/// completes. The function signature of the handler must be:
/// @code
/// void handler(
///   // Result of operation.
///   const error_code& error,
/// );
/// @endcode
template <typename Socket, typename DynamicBuffer, typename ExecuteHandler>
ASIOEXT_INITFN_RESULT_TYPE(ExecuteHandler, void(error_code))
async_execute(Socket& socket, asioext::socks::command cmd,
              const asio::ip::tcp::endpoint& remote,
              const std::string& user_id,
              ASIOEXT_MOVE_ARG(DynamicBuffer) buffer,
              ASIOEXT_MOVE_ARG(ExecuteHandler) handler);

/// @brief Asynchronously attempt to execute the given command.
///
/// This function asynchronously executes the given SOCKS command.
/// The following commands are supported by a SOCKS v4 server:
///
/// * @c connect: Establish a connection to the given remote endpoint.
/// Any data sent/received on the socket after successful command execution
/// belongs to the proxied connection.
///
/// @param socket A socket connected to the SOCKS server.
/// @param cmd The command to execute.
/// @param remote Hostname of the remote endpoint.
/// @param port Port of the remote endpoint.
/// @param user_id An opqaue user id string that will be sent to the SOCKS server.
/// @param buffer A linear_buffer that is used to buffer sent/received messages.
/// @param handler The handler to be called when the execute operation
/// completes. The function signature of the handler must be:
/// @code
/// void handler(
///   // Result of operation.
///   const error_code& error,
/// );
/// @endcode
template <typename Socket, typename DynamicBuffer, typename ExecuteHandler>
ASIOEXT_INITFN_RESULT_TYPE(ExecuteHandler, void(error_code))
async_execute(Socket& socket, asioext::socks::command cmd,
              const std::string& remote, uint16_t port,
              const std::string& user_id,
              ASIOEXT_MOVE_ARG(DynamicBuffer) buffer,
              ASIOEXT_MOVE_ARG(ExecuteHandler) handler);

/// @}

/// @ingroup net_socks
/// @defgroup net_socks_v5 SOCKS5 functions
/// SOCKS5 implementation
/// @{

/// @brief Asynchronously perform a SOCKS 5 greeting with the remote
/// SOCKS server.
///
/// This function attempts to perform a SOCKS 5 greeting on the given
/// socket which needs to be connected to the remote proxy server
/// you wish to "greet".
///
/// @param socket The connected socket. The remote endpoint
/// needs to be a SOCKS 5 proxy.
/// @param auth_methods Array containing the allowed authentication methods.
/// @param num_auth_methods Number of authentication methods entries.
/// @param buffer A DynamicBuffer that is used to buffer sent/received messages.
/// @param handler The handler to be called when the greet operation
/// completes. The function signature of the handler must be:
/// @code
/// void handler(
///   // Result of operation.
///   const error_code& error,
///
///   // Authentication method chosen by the remote end.
///   auth_method chosen_method
/// );
/// @endcode
template <typename Socket, typename DynamicBuffer, typename GreetHandler>
ASIOEXT_INITFN_RESULT_TYPE(GreetHandler, void(error_code, auth_method))
async_greet(Socket& socket,
            const auth_method* auth_methods,
            std::size_t num_auth_methods,
            ASIOEXT_MOVE_ARG(DynamicBuffer) buffer,
            ASIOEXT_MOVE_ARG(GreetHandler) handler);

/// @brief Asynchronously attempt a login on the remote SOCKS 5 server.
///
/// This function attempts to log in to the remote SOCKS 5 server with the
/// given credentials.
/// The given socket must be connected to a SOCKS 5 proxy server and the
/// initial greeting must have resulted in the user/password authentication
/// scheme being chosen.
///
/// @param socket The connected socket. The remote endpoint
/// needs to be a successfully greeted SOCKS 5 proxy.
/// @param username Username on the remote end.
/// @param password Password of the remote user.
/// @param buffer A linear_buffer that is used to buffer sent/received messages.
/// @param handler The handler to be called when the login operation
/// completes. The function signature of the handler must be:
/// @code
/// void handler(
///   // Result of operation.
///   const error_code& error,
/// );
/// @endcode
template <typename Socket, typename DynamicBuffer, typename LoginHandler>
ASIOEXT_INITFN_RESULT_TYPE(LoginHandler, void(error_code))
async_login(Socket& socket,
            const std::string& username,
            const std::string& password,
            ASIOEXT_MOVE_ARG(DynamicBuffer) buffer,
            ASIOEXT_MOVE_ARG(LoginHandler) handler);

/// @brief Asynchronously attempt to execute the given command.
///
/// This function asynchronously executes the given SOCKS command.
/// The following commands are supported by a SOCKS 5 server:
///
/// * @c connect: Establish a connection to the given remote endpoint.
/// Any data sent/received on the socket after successful command execution
/// belongs to the proxied connection.
///
/// @param socket The connected socket. The remote endpoint
/// needs to be a successfully greeted SOCKS 5 proxy.
/// @param cmd The command to execute.
/// @param remote The remote endpoint.
/// @param buffer A linear_buffer that is used to buffer sent/received messages.
/// @param handler The handler to be called when the execute operation
/// completes. The function signature of the handler must be:
/// @code
/// void handler(
///   // Result of operation.
///   const error_code& error,
/// );
/// @endcode
template <typename Socket, typename DynamicBuffer, typename ExecuteHandler>
ASIOEXT_INITFN_RESULT_TYPE(ExecuteHandler, void(error_code))
async_execute(Socket& socket, command cmd,
              const asio::ip::tcp::endpoint& remote,
              ASIOEXT_MOVE_ARG(DynamicBuffer) buffer,
              ASIOEXT_MOVE_ARG(ExecuteHandler) handler);

/// @brief Asynchronously attempt to execute the given command.
///
/// This function asynchronously executes the given SOCKS command.
/// The following commands are supported by a SOCKS 5 server:
///
/// * @c connect: Establish a connection to the given remote endpoint.
/// Any data sent/received on the socket after successful command execution
/// belongs to the proxied connection.
///
/// @param socket The connected socket. The remote endpoint
/// needs to be a successfully greeted SOCKS 5 proxy.
/// @param cmd The command to execute.
/// @param remote Hostname of the remote endpoint.
/// @param port Port of the remote endpoint.
/// @param buffer A linear_buffer that is used to buffer sent/received messages.
/// @param handler The handler to be called when the execute operation
/// completes. The function signature of the handler must be:
/// @code
/// void handler(
///   // Result of operation.
///   const error_code& error,
/// );
/// @endcode
template <typename Socket, typename DynamicBuffer, typename ExecuteHandler>
ASIOEXT_INITFN_RESULT_TYPE(ExecuteHandler, void(error_code))
async_execute(Socket& socket, command cmd,
              const std::string& remote, uint16_t port,
              ASIOEXT_MOVE_ARG(DynamicBuffer) buffer,
              ASIOEXT_MOVE_ARG(ExecuteHandler) handler);

/// @}

}

ASIOEXT_NS_END

#include "asioext/socks/impl/client.hpp"

#endif
