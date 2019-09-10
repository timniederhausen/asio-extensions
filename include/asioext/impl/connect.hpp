/// @copyright Copyright (c) 2017 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_IMPL_CONNECT_HPP
#define ASIOEXT_IMPL_CONNECT_HPP

#include "asioext/compose.hpp"
#include "asioext/bind_handler.hpp"
#include "asioext/detail/coroutine.hpp"

#if defined(ASIOEXT_USE_BOOST_ASIO)
# include <boost/asio/connect.hpp>
#else
# include <asio/connect.hpp>
#endif

ASIOEXT_NS_BEGIN

namespace detail {

class connect_op : asio::coroutine
{
public:
  connect_op(asio::ip::tcp::socket::lowest_layer_type& socket)
    : socket_(socket)
  {}

  template <typename Self>
  void operator()(Self& self, error_code ec,
                  asio::ip::tcp::resolver::iterator iter);

private:
  asio::ip::tcp::socket::lowest_layer_type& socket_;
};

template <typename Self>
void connect_op::operator()(Self& self, error_code ec,
                            asio::ip::tcp::resolver::iterator iter)
{
  if (ec) {
    self.complete(ec, iter);
    return;
  }

  ASIOEXT_CORO_REENTER (this) {
    ASIOEXT_CORO_YIELD asio::async_connect(socket_, iter, std::move(self));
    self.complete(ec, iter);
  }
}

struct initiate_connect
{
  template <typename Handler>
  void operator()(Handler&& handler,
                  asio::ip::tcp::socket::lowest_layer_type& socket,
                  asio::ip::tcp::resolver& resolver,
                  const asio::ip::tcp::resolver::query& q)
  {
    error_code ec;
    // Open the socket to give the caller something to close to cancel the
    // asynchronous operation.
    socket.open(asio::ip::tcp::v4(), ec);
    if (ec) {
      asio::post(bind_handler(handler, ec,
                              asio::ip::tcp::resolver::iterator()));
      return;
    }

    resolver.async_resolve(q, make_composed_operation(connect_op(socket),
                                                      std::move(handler)));
  }
};

}

template <typename ComposedConnectHandler>
ASIOEXT_INITFN_RESULT_TYPE(ComposedConnectHandler,
                           void(error_code, asio::ip::tcp::resolver::iterator))
async_connect(asio::ip::tcp::socket::lowest_layer_type& socket,
              asio::ip::tcp::resolver& resolver,
              const asio::ip::tcp::resolver::query& q,
              ComposedConnectHandler&& handler)
{
  return async_initiate<ComposedConnectHandler,
    void(error_code, asio::ip::tcp::resolver::iterator)>(
      detail::initiate_connect{}, handler, socket, resolver, q);
}

ASIOEXT_NS_END

#endif
