/// @copyright Copyright (c) 2017 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#include "asioext/connect.hpp"

#include "asioext/detail/throw_error.hpp"

ASIOEXT_NS_BEGIN

void connect(asio::ip::tcp::socket::lowest_layer_type& socket,
             asio::ip::tcp::resolver& resolver,
             const asio::ip::tcp::resolver::query& q)
{
  error_code ec;
  connect(socket, resolver, q, ec);
  detail::throw_error(ec, "connect");
}

void connect(asio::ip::tcp::socket::lowest_layer_type& socket,
             asio::ip::tcp::resolver& resolver,
             const asio::ip::tcp::resolver::query& q,
             error_code& ec)
{
  // Get a list of endpoints corresponding to the query.
  asio::ip::tcp::resolver::iterator iter = resolver.resolve(q, ec);
  if (ec)
    return;

  // Try each endpoint until we successfully establish a connection.
  ec = asio::error::host_not_found;
  while (ec && iter != boost::asio::ip::tcp::resolver::iterator()) {
    socket.close(ec);
    socket.connect(*iter++, ec);
  }
}

ASIOEXT_NS_END
