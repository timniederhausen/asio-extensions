/// @copyright Copyright (c) 2018 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_SOCKS_IMPL_CLIENT_HPP
#define ASIOEXT_SOCKS_IMPL_CLIENT_HPP

#include "asioext/socks/detail/client.hpp"

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
  typedef async_completion<ExecuteHandler, void (error_code)> init_t;

  init_t init(handler);
  detail::v4_exec_op<Socket, DynamicBuffer,
      typename init_t::completion_handler_type> op(
    init.completion_handler, socket, cmd, remote, std::string(), 0,
    user_id, buffer);

  return init.result.get();
}

template <typename Socket, typename DynamicBuffer, typename ExecuteHandler>
ASIOEXT_INITFN_RESULT_TYPE(ExecuteHandler, void(error_code))
async_execute(Socket& socket, command cmd,
              const std::string& remote, uint16_t port,
              const std::string& user_id,
              ASIOEXT_MOVE_ARG(DynamicBuffer) buffer,
              ASIOEXT_MOVE_ARG(ExecuteHandler) handler)
{
  typedef async_completion<ExecuteHandler, void (error_code)> init_t;

  init_t init(handler);
  detail::v4_exec_op<Socket, DynamicBuffer,
      typename init_t::completion_handler_type> op(
    init.completion_handler, socket, cmd, asio::ip::tcp::endpoint(),
    remote, port, user_id, buffer);

  return init.result.get();
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
  typedef async_completion<
    GreetHandler, void (error_code, auth_method)
  > init_t;

  init_t init(handler);
  detail::v5_greet_op<Socket, DynamicBuffer,
      typename init_t::completion_handler_type> op(
    init.completion_handler, socket, auth_methods, num_auth_methods, buffer);

  return init.result.get();
}

template <typename Socket, typename DynamicBuffer, typename LoginHandler>
ASIOEXT_INITFN_RESULT_TYPE(LoginHandler, void(error_code))
async_login(Socket& socket,
            const std::string& username,
            const std::string& password,
            ASIOEXT_MOVE_ARG(DynamicBuffer) buffer,
            ASIOEXT_MOVE_ARG(LoginHandler) handler)
{
  typedef async_completion<LoginHandler, void (error_code)> init_t;

  init_t init(handler);
  detail::v5_login_op<Socket, DynamicBuffer,
      typename init_t::completion_handler_type> op(
    init.completion_handler, socket, username, password, buffer);

  return init.result.get();
}

template <typename Socket, typename DynamicBuffer, typename ExecuteHandler>
ASIOEXT_INITFN_RESULT_TYPE(ExecuteHandler, void(error_code))
async_execute(Socket& socket, command cmd,
              const asio::ip::tcp::endpoint& remote,
              ASIOEXT_MOVE_ARG(DynamicBuffer) buffer,
              ASIOEXT_MOVE_ARG(ExecuteHandler) handler)
{
  typedef async_completion<ExecuteHandler, void (error_code)> init_t;

  init_t init(handler);
  detail::v5_exec_op<Socket, DynamicBuffer,
      typename init_t::completion_handler_type> op(
    init.completion_handler, socket, cmd, remote, std::string(), 0,
    buffer);

  return init.result.get();
}

template <typename Socket, typename DynamicBuffer, typename ExecuteHandler>
ASIOEXT_INITFN_RESULT_TYPE(ExecuteHandler, void(error_code))
async_execute(Socket& socket, command cmd,
              const std::string& remote, uint16_t port,
              ASIOEXT_MOVE_ARG(DynamicBuffer) buffer,
              ASIOEXT_MOVE_ARG(ExecuteHandler) handler)
{
  typedef async_completion<ExecuteHandler, void (error_code)> init_t;

  init_t init(handler);
  detail::v5_exec_op<Socket, DynamicBuffer,
      typename init_t::completion_handler_type> op(
    init.completion_handler, socket, cmd, asio::ip::tcp::endpoint(),
    remote, port, buffer);

  return init.result.get();
}

}

ASIOEXT_NS_END

#endif
