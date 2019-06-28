/// @copyright Copyright (c) 2018 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_SOCKS_IMPL_CLIENT_HPP
#define ASIOEXT_SOCKS_IMPL_CLIENT_HPP

#include "asioext/socks/detail/client.hpp"
#include "asioext/compose.hpp"

ASIOEXT_NS_BEGIN

namespace socks {

// SOCKS4a

template <typename Socket, typename DynamicBuffer, typename ExecuteHandler>
ASIOEXT_INITFN_RESULT_TYPE(ExecuteHandler, void(error_code))
async_execute(Socket& socket, command cmd,
              const asio::ip::tcp::endpoint& remote,
              const std::string& user_id,
              ASIOEXT_MOVE_ARG(DynamicBuffer) buffer,
              ASIOEXT_MOVE_ARG(ExecuteHandler) handler)
{
  return async_compose<ExecuteHandler, void (error_code)>(
      detail::v4_exec_op<Socket, DynamicBuffer>(
          socket, cmd, remote, std::string(), 0, user_id, buffer),
      handler, socket
  );
}

template <typename Socket, typename DynamicBuffer, typename ExecuteHandler>
ASIOEXT_INITFN_RESULT_TYPE(ExecuteHandler, void(error_code))
async_execute(Socket& socket, command cmd,
              const std::string& remote, uint16_t port,
              const std::string& user_id,
              ASIOEXT_MOVE_ARG(DynamicBuffer) buffer,
              ASIOEXT_MOVE_ARG(ExecuteHandler) handler)
{
  return async_compose<ExecuteHandler, void (error_code)>(
      detail::v4_exec_op<Socket, DynamicBuffer>(
          socket, cmd, asio::ip::tcp::endpoint(), remote, 0, user_id, buffer),
      handler, socket
  );
}

// SOCKS5

template <typename Socket, typename DynamicBuffer, typename GreetHandler>
ASIOEXT_INITFN_RESULT_TYPE(GreetHandler, void(error_code, auth_method))
async_greet(Socket& socket,
            const auth_method* auth_methods,
            std::size_t num_auth_methods,
            ASIOEXT_MOVE_ARG(DynamicBuffer) buffer,
            ASIOEXT_MOVE_ARG(GreetHandler) handler)
{
  return async_compose<GreetHandler, void (error_code, auth_method)>(
      detail::v5_greet_op<Socket, DynamicBuffer>(
        socket, auth_methods, num_auth_methods, buffer),
      handler, socket
  );
}

template <typename Socket, typename DynamicBuffer, typename LoginHandler>
ASIOEXT_INITFN_RESULT_TYPE(LoginHandler, void(error_code))
async_login(Socket& socket,
            const std::string& username,
            const std::string& password,
            ASIOEXT_MOVE_ARG(DynamicBuffer) buffer,
            ASIOEXT_MOVE_ARG(LoginHandler) handler)
{
  return async_compose<LoginHandler, void (error_code)>(
      detail::v5_login_op<Socket, DynamicBuffer>(
        socket, username, password, buffer),
      handler, socket
  );
}

template <typename Socket, typename DynamicBuffer, typename ExecuteHandler>
ASIOEXT_INITFN_RESULT_TYPE(ExecuteHandler, void(error_code))
async_execute(Socket& socket, command cmd,
              const asio::ip::tcp::endpoint& remote,
              ASIOEXT_MOVE_ARG(DynamicBuffer) buffer,
              ASIOEXT_MOVE_ARG(ExecuteHandler) handler)
{
  return async_compose<ExecuteHandler, void (error_code)>(
      detail::v5_exec_op<Socket, DynamicBuffer>(
        socket, cmd, remote, std::string(), 0, buffer),
      handler, socket
  );
}

template <typename Socket, typename DynamicBuffer, typename ExecuteHandler>
ASIOEXT_INITFN_RESULT_TYPE(ExecuteHandler, void(error_code))
async_execute(Socket& socket, command cmd,
              const std::string& remote, uint16_t port,
              ASIOEXT_MOVE_ARG(DynamicBuffer) buffer,
              ASIOEXT_MOVE_ARG(ExecuteHandler) handler)
{
  return async_compose<ExecuteHandler, void (error_code)>(
      detail::v5_exec_op<Socket, DynamicBuffer>(
        socket, cmd, asio::ip::tcp::endpoint(), remote, port, buffer),
      handler, socket
  );
}

}

ASIOEXT_NS_END

#endif
