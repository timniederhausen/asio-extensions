/// @copyright Copyright (c) 2017 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#include "asioext/connect.hpp"

#include "asioext/detail/throw_error.hpp"

ASIOEXT_NS_BEGIN

void connect(asio::ip::tcp::socket::lowest_layer_type& socket,
             asio::ip::tcp::resolver& resolver,
             std::string_view host,
             std::string_view service,
             asio::ip::resolver_base::flags resolve_flags)
{
  error_code ec;
  connect(socket, resolver, host, service, resolve_flags, ec);
  detail::throw_error(ec, "connect");
}

void connect(asio::ip::tcp::socket::lowest_layer_type& socket,
             asio::ip::tcp::resolver& resolver,
             std::string_view host,
             std::string_view service,
             asio::ip::resolver_base::flags resolve_flags,
             error_code& ec)
{
  // Get a list of endpoints corresponding to the query.
  const asio::ip::tcp::resolver::results_type results = resolver.resolve(host, service, resolve_flags, ec);
  if (ec)
    return;

  // Try each endpoint until we successfully establish a connection.
  ec = asio::error::host_not_found;
  for (const auto& entry : results) {
    socket.close(ec);
    socket.connect(entry, ec);
    if (!ec)
      return;
  }
}

ASIOEXT_NS_END
