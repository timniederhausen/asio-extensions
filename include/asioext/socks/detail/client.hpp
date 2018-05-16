/// @copyright Copyright (c) 2018 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_SOCKS_DETAIL_CLIENT_HPP
#define ASIOEXT_SOCKS_DETAIL_CLIENT_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include "asioext/socks/error.hpp"
#include "asioext/composed_operation.hpp"
#include "asioext/bind_handler.hpp"

#include "asioext/socks/detail/protocol.hpp"
#include "asioext/detail/coroutine.hpp"
#include "asioext/detail/move_support.hpp"

#if defined(ASIOEXT_USE_BOOST_ASIO)
# include <boost/asio/read.hpp>
# include <boost/asio/write.hpp>
#else
# include <asio/read.hpp>
# include <asio/write.hpp>
#endif

ASIOEXT_NS_BEGIN

namespace socks {
namespace detail {

template <typename Socket, typename DynamicBuffer, typename Handler>
class socks_sgreet_op : asio::coroutine
{
public:
  socks_sgreet_op(Handler& handler, Socket& socket,
                  const auth_method* auth_methods,
                  std::size_t num_auth_methods,
                  DynamicBuffer& buffer)
    : socket_(socket)
    , buffer_(ASIOEXT_MOVE_CAST(DynamicBuffer)(buffer))
  {
    const std::size_t size =
        get_sgreet_packet_size(auth_methods, num_auth_methods);

    if (0 == size) {
      socket_.get_io_service().post(asioext::bind_handler(
          ASIOEXT_MOVE_CAST(Handler)(handler), asio::error::invalid_argument,
          auth_method::no_acceptable));
      return;
    }

    asio::mutable_buffer buf = buffer_.prepare(size);
    encode_sgreet_packet(auth_methods, num_auth_methods,
                         asio::buffer_cast<uint8_t*>(buf));
    buffer_.commit(size);

    asio::async_write(
        socket, buffer_.data(),
        asioext::make_composed_operation(
            ASIOEXT_MOVE_CAST(Handler)(handler),
            ASIOEXT_MOVE_CAST(socks_sgreet_op)(*this)));
  }

  void operator()(ASIOEXT_MOVE_ARG(Handler) handler, error_code ec,
                  std::size_t size = 0);

private:
  Socket& socket_;
  DynamicBuffer buffer_;
};

template <typename Socket, typename DynamicBuffer, typename Handler>
void socks_sgreet_op<Socket, DynamicBuffer, Handler>::operator()(
    ASIOEXT_MOVE_ARG(Handler) handler, error_code ec, std::size_t size)
{
  if (ec) {
    handler(ec, auth_method::no_acceptable);
    return;
  }

  ASIOEXT_CORO_REENTER (this) {
    buffer_.consume(size);
    ASIOEXT_CORO_YIELD asio::async_read(
        socket_, buffer_.prepare(2),
        asioext::make_composed_operation(
            ASIOEXT_MOVE_CAST(Handler)(handler),
            ASIOEXT_MOVE_CAST(socks_sgreet_op)(*this)));

    if (ec) {
      handler(ec, auth_method::no_acceptable);
      return;
    }

    buffer_.commit(2);

    const uint8_t* data = asio::buffer_cast<const uint8_t*>(buffer_.data());

    const uint8_t version = data[0];
    const auth_method chosen_method = static_cast<auth_method>(data[1]);

    buffer_.consume(2);

    if (version != 5) {
      handler(error::invalid_version, auth_method::no_acceptable);
      return;
    }

    if (chosen_method == auth_method::no_acceptable) {
      handler(error::no_acceptable_auth_method, auth_method::no_acceptable);
      return;
    }

    handler(ec, chosen_method);
  }
}

template <typename Socket, typename DynamicBuffer, typename Handler>
class socks_slogin_op : asio::coroutine
{
public:
  socks_slogin_op(Handler& handler, Socket& socket,
                  const std::string& username,
                  const std::string& password,
                  DynamicBuffer& buffer)
    : socket_(socket)
    , buffer_(ASIOEXT_MOVE_CAST(DynamicBuffer)(buffer))
  {
    const std::size_t size =
        get_slogin_packet_size(username, password);

    if (0 == size) {
      socket_.get_io_service().post(asioext::bind_handler(
          ASIOEXT_MOVE_CAST(Handler)(handler), asio::error::invalid_argument));
      return;
    }

    asio::mutable_buffer buf = buffer_.prepare(size);
    encode_slogin_packet(username, password,
                         asio::buffer_cast<uint8_t*>(buf));
    buffer_.commit(size);

    asio::async_write(socket, buffer_.data(),
        asioext::make_composed_operation(
            ASIOEXT_MOVE_CAST(Handler)(handler),
            ASIOEXT_MOVE_CAST(socks_slogin_op)(*this)));
  }

  void operator()(ASIOEXT_MOVE_ARG(Handler) handler, error_code ec,
                  std::size_t size = 0);

private:
  Socket& socket_;
  DynamicBuffer buffer_;
};

template <typename Socket, typename DynamicBuffer, typename Handler>
void socks_slogin_op<Socket, DynamicBuffer, Handler>::operator()(
    ASIOEXT_MOVE_ARG(Handler) handler, error_code ec, std::size_t size)
{
  if (ec) {
    handler(ec);
    return;
  }

  ASIOEXT_CORO_REENTER (this) {
    buffer_.consume(size);
    ASIOEXT_CORO_YIELD asio::async_read(
        socket_, buffer_.prepare(2),
        asioext::make_composed_operation(
            ASIOEXT_MOVE_CAST(Handler)(handler),
            ASIOEXT_MOVE_CAST(socks_slogin_op)(*this)));

    if (ec) {
      handler(ec);
      return;
    }

    buffer_.commit(2);

    const uint8_t* data = asio::buffer_cast<const uint8_t*>(buffer_.data());

    const uint8_t version = data[0];
    const uint8_t status_code = data[1];

    buffer_.consume(2);

    if (version != 1) {
      handler(error::invalid_auth_version);
      return;
    }

    if (status_code != 0) {
      handler(error::login_failed);
      return;
    }

    handler(ec);
  }
}

template <typename Socket, typename DynamicBuffer, typename Handler>
class socks_sexec_op : asio::coroutine
{
public:
  socks_sexec_op(Handler& handler, Socket& socket,
                 command cmd,
                 const asio::ip::tcp::endpoint& remote,
                 const std::string& remote_host,
                 uint16_t port,
                 DynamicBuffer& buffer)
    : socket_(socket)
    , buffer_(ASIOEXT_MOVE_CAST(DynamicBuffer)(buffer))
  {
    const std::size_t size =
        get_sexec_packet_size(cmd, remote, remote_host, port);

    if (0 == size) {
      socket_.get_io_service().post(asioext::bind_handler(
          ASIOEXT_MOVE_CAST(Handler)(handler), asio::error::invalid_argument));
      return;
    }

    asio::mutable_buffer buf = buffer_.prepare(size);
    encode_sexec_packet(cmd, remote, remote_host, port,
                        asio::buffer_cast<uint8_t*>(buf));
    buffer_.commit(size);

    asio::async_write(
        socket, buffer_.data(),
        asioext::make_composed_operation(
            ASIOEXT_MOVE_CAST(Handler)(handler),
            ASIOEXT_MOVE_CAST(socks_sexec_op)(*this)));
  }

  void operator()(ASIOEXT_MOVE_ARG(Handler) handler, error_code ec,
                  std::size_t size = 0);

private:
  Socket& socket_;
  DynamicBuffer buffer_;
  uint8_t address_type_;
  uint8_t first_address_byte_;
};

template <typename Socket, typename DynamicBuffer, typename Handler>
void socks_sexec_op<Socket, DynamicBuffer, Handler>::operator()(
    ASIOEXT_MOVE_ARG(Handler) handler, error_code ec, std::size_t size)
{
  if (ec) {
    handler(ec);
    return;
  }

  ASIOEXT_CORO_REENTER (this) {
    buffer_.consume(size);
    ASIOEXT_CORO_YIELD asio::async_read(
        socket_, buffer_.prepare(5),
        asioext::make_composed_operation(
            ASIOEXT_MOVE_CAST(Handler)(handler),
            ASIOEXT_MOVE_CAST(socks_sexec_op)(*this)));

    if (ec) {
      handler(ec);
      return;
    }

    {
      buffer_.commit(5);

      const uint8_t* data = asio::buffer_cast<const uint8_t*>(buffer_.data());

      const uint8_t version = data[0];
      const uint8_t status_code = data[1];

      address_type_ = data[3];
      first_address_byte_ = data[4];

      buffer_.consume(5);

      if (version != 5) {
        handler(error::invalid_version);
        return;
      }

      if (status_code != 0) {
        switch (status_code) {
          case 2: ec = asio::error::no_permission; break;
          case 3: ec = asio::error::network_unreachable; break;
          case 4: ec = asio::error::host_unreachable; break;
          case 5: ec = asio::error::connection_refused; break;
          case 6: ec = asio::error::timed_out; break;
          case 7: ec = error::command_not_supported; break;
          case 8: ec = asio::error::address_family_not_supported; break;
        }
        handler(ec);
        return;
      }
    }

    switch (address_type_) {
      // IPv4
      case 1: {
        size = 4 - 1;
        break;
      }
      // Hostname
      case 3: {
        size = first_address_byte_;
        break;
      }
      // IPv6
      case 4: {
        size = 16 - 1;
        break;
      }
    }

    ASIOEXT_CORO_YIELD asio::async_read(
        socket_, buffer_.prepare(size + 2),
        asioext::make_composed_operation(
            ASIOEXT_MOVE_CAST(Handler)(handler),
            ASIOEXT_MOVE_CAST(socks_sexec_op)(*this)));

    if (ec) {
      handler(ec);
      return;
    }

    // TODO: should we return the actual target endpoint to the user?
    buffer_.commit(size);
    buffer_.consume(size);

    handler(ec);
  }
}

}
}

ASIOEXT_NS_END

#endif
