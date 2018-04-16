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
#include "asioext/linear_buffer.hpp"

#include "asioext/socks/detail/protocol.hpp"
#include "asioext/detail/coroutine.hpp"
#include "asioext/detail/bind_handler.hpp"
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

template <class Socket, class Handler>
class socks_sgreet_op
  : public composed_operation<Handler>
  , asio::coroutine
{
public:
  socks_sgreet_op(Handler& handler, Socket& socket,
                  const auth_method* auth_methods,
                  std::size_t num_auth_methods,
                  linear_buffer& buffer)
    : composed_operation<Handler>(ASIOEXT_MOVE_CAST(Handler)(handler))
    , socket_(socket)
    , buffer_(buffer)
  {
    if (!encode_sgreet_packet(auth_methods, num_auth_methods, buffer)) {
      socket_.get_io_service().post(::asioext::detail::bind_handler(
          this->handler_, asio::error::invalid_argument,
          auth_method::no_acceptable));
      return;
    }

    asio::async_write(socket, buffer.data(),
                      ASIOEXT_MOVE_CAST(socks_sgreet_op)(*this));
  }

  void operator()(error_code ec, std::size_t bytes_transferred = 0);

private:
  Socket& socket_;
  linear_buffer& buffer_;
};

template <class Socket, class Handler>
void socks_sgreet_op<Socket, Handler>::operator()(
    error_code ec, std::size_t bytes_transferred)
{
  if (ec) {
    this->handler_(ec, auth_method::no_acceptable);
    return;
  }

  ASIOEXT_CORO_REENTER (this) {
    buffer_.consume(bytes_transferred);
    ASIOEXT_CORO_YIELD asio::async_read(
        socket_, buffer_.prepare(2),
        ASIOEXT_MOVE_CAST(socks_sgreet_op)(*this));

    if (ec) {
      this->handler_(ec, auth_method::no_acceptable);
      return;
    }

    buffer_.commit(2);

    const uint8_t* data = asio::buffer_cast<const uint8_t*>(buffer_.data());

    const uint8_t version = data[0];
    const auth_method chosen_method = static_cast<auth_method>(data[1]);

    buffer_.consume(2);

    if (version != 5) {
      this->handler_(error::invalid_version,
                     auth_method::no_acceptable);
      return;
    }

    if (chosen_method == auth_method::no_acceptable) {
      this->handler_(error::no_acceptable_auth_method,
                     auth_method::no_acceptable);
      return;
    }

    this->handler_(ec, chosen_method);
  }
}

template <class Socket, class Handler>
class socks_slogin_op
  : public composed_operation<Handler>
  , asio::coroutine
{
public:
  socks_slogin_op(Handler& handler, Socket& socket,
                  const std::string& username,
                  const std::string& password,
                  linear_buffer& buffer)
    : composed_operation<Handler>(ASIOEXT_MOVE_CAST(Handler)(handler))
    , socket_(socket)
    , buffer_(buffer)
  {
    if (!encode_slogin_packet(username, password, buffer)) {
      socket_.get_io_service().post(::asioext::detail::bind_handler(
          this->handler_, asio::error::invalid_argument));
      return;
    }

    asio::async_write(socket, buffer.data(),
                      ASIOEXT_MOVE_CAST(socks_slogin_op)(*this));
  }

  void operator()(error_code ec, std::size_t bytes_transferred = 0);

private:
  Socket& socket_;
  linear_buffer& buffer_;
};

template <class Socket, class Handler>
void socks_slogin_op<Socket, Handler>::operator()(
    error_code ec, std::size_t bytes_transferred)
{
  if (ec) {
    this->handler_(ec);
    return;
  }

  ASIOEXT_CORO_REENTER (this) {
    buffer_.consume(bytes_transferred);
    ASIOEXT_CORO_YIELD asio::async_read(
        socket_, buffer_.prepare(2),
        ASIOEXT_MOVE_CAST(socks_slogin_op)(*this));

    if (ec) {
      this->handler_(ec);
      return;
    }

    buffer_.commit(2);

    const uint8_t* data = asio::buffer_cast<const uint8_t*>(buffer_.data());

    const uint8_t version = data[0];
    const uint8_t status_code = data[1];

    buffer_.consume(2);

    if (version != 1) {
      this->handler_(error::invalid_auth_version);
      return;
    }

    if (status_code != 0) {
      this->handler_(error::login_failed);
      return;
    }

    this->handler_(ec);
  }
}

template <class Socket, class Handler>
class socks_sexec_op
  : public composed_operation<Handler>
  , asio::coroutine
{
public:
  socks_sexec_op(Handler& handler, Socket& socket,
                 command cmd,
                 const asio::ip::tcp::endpoint& remote,
                 const std::string& remote_host,
                 uint16_t port,
                 linear_buffer& buffer)
    : composed_operation<Handler>(ASIOEXT_MOVE_CAST(Handler)(handler))
    , socket_(socket)
    , buffer_(buffer)
  {
    if (!encode_sexec_packet(cmd, remote, remote_host, port, buffer)) {
      socket_.get_io_service().post(::asioext::detail::bind_handler(
          this->handler_, asio::error::invalid_argument));
      return;
    }

    asio::async_write(socket, buffer.data(),
                      ASIOEXT_MOVE_CAST(socks_sexec_op)(*this));
  }

  void operator()(error_code ec, std::size_t bytes_transferred = 0);

private:
  Socket& socket_;
  linear_buffer& buffer_;
  uint8_t address_type_;
  uint8_t first_address_byte_;
};

template <class Socket, class Handler>
void socks_sexec_op<Socket, Handler>::operator()(
    error_code ec, std::size_t bytes_transferred)
{
  if (ec) {
    this->handler_(ec);
    return;
  }

  std::size_t size;
  ASIOEXT_CORO_REENTER (this) {
    buffer_.consume(bytes_transferred);
    ASIOEXT_CORO_YIELD asio::async_read(
        socket_, buffer_.prepare(5),
        ASIOEXT_MOVE_CAST(socks_sexec_op)(*this));

    if (ec) {
      this->handler_(ec);
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
        this->handler_(error::invalid_version);
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
        this->handler_(ec);
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
        ASIOEXT_MOVE_CAST(socks_sexec_op)(*this));

    if (ec) {
      this->handler_(ec);
      return;
    }

    buffer_.commit(bytes_transferred);
    buffer_.consume(bytes_transferred);

    this->handler_(ec);
  }
}

}
}

ASIOEXT_NS_END

#endif
