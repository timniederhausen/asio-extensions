/// @copyright Copyright (c) 2017 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_IMPL_CONNECT_HPP
#define ASIOEXT_IMPL_CONNECT_HPP

#include "asioext/composed_operation.hpp"
#include "asioext/bind_handler.hpp"

#if defined(ASIOEXT_USE_BOOST_ASIO)
# include <boost/asio/connect.hpp>
#else
# include <asio/connect.hpp>
#endif

ASIOEXT_NS_BEGIN

namespace detail {

template <class Handler>
class connect_op
{
public:
  connect_op(Handler& handler,
             asio::ip::tcp::socket::lowest_layer_type& socket,
             asio::ip::tcp::resolver& resolver,
             const asio::ip::tcp::resolver::query& q)
    : socket_(socket)
  {
    // Open the socket to give the caller something to close to cancel the
    // asynchronous operation.
    error_code ec;
    socket_.open(asio::ip::tcp::v4(), ec);
    if (ec) {
      socket_.get_io_service().post(bind_handler(
          ASIOEXT_MOVE_CAST(Handler)(handler), ec,
          asio::ip::tcp::resolver::iterator()));
      return;
    }
    resolver.async_resolve(q, asioext::make_composed_operation(
        ASIOEXT_MOVE_CAST(Handler)(handler),
        ASIOEXT_MOVE_CAST(connect_op)(*this)));
  }

  void operator()(ASIOEXT_MOVE_ARG(Handler) handler,
                  error_code ec, asio::ip::tcp::resolver::iterator iter);

private:
  asio::ip::tcp::socket::lowest_layer_type& socket_;
};

template <class Handler>
void connect_op<Handler>::operator()(ASIOEXT_MOVE_ARG(Handler) handler,
                                     error_code ec,
                                     asio::ip::tcp::resolver::iterator iter)
{
  if (!ec) {
    asio::async_connect(socket_, iter, ASIOEXT_MOVE_CAST(Handler)(handler));
    return;
  }
  handler(ec, asio::ip::tcp::resolver::iterator());
}

}

template <typename ComposedConnectHandler>
ASIOEXT_INITFN_RESULT_TYPE(ComposedConnectHandler,
                           void(error_code, asio::ip::tcp::resolver::iterator))
async_connect(asio::ip::tcp::socket::lowest_layer_type& socket,
              asio::ip::tcp::resolver& resolver,
              const asio::ip::tcp::resolver::query& q,
              ASIOEXT_MOVE_ARG(ComposedConnectHandler) handler)
{
  typedef async_completion<
    ComposedConnectHandler,
    void (error_code, asio::ip::tcp::resolver::iterator)
  > init_t;

  init_t init(handler);
  detail::connect_op<typename init_t::completion_handler_type>(
    init.completion_handler, socket, resolver, q);
  return init.result.get();
}

ASIOEXT_NS_END

#endif
