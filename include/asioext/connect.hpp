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
#  pragma once
#endif

#include "asioext/error_code.hpp"

#if defined(ASIOEXT_USE_BOOST_ASIO)
#  include <boost/asio/ip/tcp.hpp>
#  include <boost/asio/compose.hpp>
#  include <boost/asio/connect.hpp>
#else
#  include <asio/ip/tcp.hpp>
#  include <asio/compose.hpp>
#  include <asio/connect.hpp>
#endif

ASIOEXT_NS_BEGIN

namespace detail {

template <typename Protocol, typename Executor>
class connect_op
{
public:
  connect_op(asio::basic_socket<Protocol, Executor>& socket,
             asio::ip::basic_resolver<Protocol>& resolver,
             std::string_view host,
             std::string_view service,
             asio::ip::resolver_base::flags resolve_flags)
    : socket_(socket)
    , resolver_(resolver)
    , host_(host)
    , service_(service)
    , resolve_flags_(resolve_flags)
  {
  }

  template <typename Self>
  void operator()(Self& self)
  {
    resolver_.async_resolve(host_, service_, resolve_flags_, std::move(self));
  }

  template <typename Self>
  void operator()(Self& self, error_code ec, asio::ip::tcp::resolver::results_type results)
  {
    if (!ec) {
      asio::async_connect(socket_, std::move(results), std::move(self));
    } else {
      self.complete(ec, typename Protocol::endpoint{});
    }
  }

  template <typename Self>
  void operator()(Self& self, error_code ec, asio::ip::tcp::resolver::endpoint_type ep)
  {
    self.complete(ec, std::move(ep));
  }

private:
  asio::basic_socket<Protocol, Executor>& socket_;
  asio::ip::basic_resolver<Protocol>& resolver_;
  // These are only valid during the initial no-args operator() call!
  std::string_view host_;
  std::string_view service_;
  asio::ip::resolver_base::flags resolve_flags_;
};

}

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
///
/// @param host A string identifying a location. May be a descriptive name or
/// a numeric address string. If an empty string and the passive flag has been
/// specified, the resolved endpoints are suitable for local service binding.
/// If an empty string and passive is not specified, the resolved endpoints
/// will use the loopback address.
///
/// @param service A string identifying the requested service. This may be a
/// descriptive name or a numeric string corresponding to a port number. May
/// be an empty string, in which case all resolved endpoints will have a port
/// number of 0.
///
/// @param resolve_flags A set of flags that determine how name resolution
/// should be performed. The default flags are suitable for communication with
/// remote hosts. See the @ref resolver_base documentation for the set of
/// available flags.
///
/// @throws asio::system_error Thrown on failure.
/// If the sequence is empty, the associated error_code is
/// @c asio::error::not_found.
/// Otherwise, contains the error from the last connection attempt.
ASIOEXT_DECL void connect(asio::ip::tcp::socket::lowest_layer_type& socket,
                          asio::ip::tcp::resolver& resolver,
                          std::string_view host,
                          std::string_view service,
                          asio::ip::resolver_base::flags resolve_flags = {});

/// @brief Establishes a socket connection by trying each endpoint of a name.
///
/// This function attempts to connect a socket by first resolving the given
/// name (using the query and resolver arguments) and then trying each
/// resolved endpoint (by calling the socket's connect member) until
/// a connection is established (or we've run out of endpoints).
///
/// @param socket The socket to be connected.
/// If the socket is already open, it will be closed.
///
/// @param resolver The resolver to be used for our query.
///
/// @param host A string identifying a location. May be a descriptive name or
/// a numeric address string. If an empty string and the passive flag has been
/// specified, the resolved endpoints are suitable for local service binding.
/// If an empty string and passive is not specified, the resolved endpoints
/// will use the loopback address.
///
/// @param service A string identifying the requested service. This may be a
/// descriptive name or a numeric string corresponding to a port number. May
/// be an empty string, in which case all resolved endpoints will have a port
/// number of 0.
///
/// @param resolve_flags A set of flags that determine how name resolution
/// should be performed. The default flags are suitable for communication with
/// remote hosts. See the @ref resolver_base documentation for the set of
/// available flags.
///
/// @param ec Set to indicate what error occurred, if any.
/// If the sequence is empty, set to @c asio::error::not_found.
/// Otherwise, contains the error from the last connection attempt.
ASIOEXT_DECL void connect(asio::ip::tcp::socket::lowest_layer_type& socket,
                          asio::ip::tcp::resolver& resolver,
                          std::string_view host,
                          std::string_view service,
                          asio::ip::resolver_base::flags resolve_flags,
                          error_code& ec);

/// @brief Asynchronously establishes a socket connection by trying each
/// endpoint of a resolved name.
///
/// This function attempts to connect a socket by first resolving the given
/// name (using the query and resolver arguments) and then trying each
/// resolved endpoint (by calling the socket's connect member) until
/// a connection is established (or we've run out of endpoints).
///
/// @param socket The socket to be connected.
/// If the socket is already open, it will be closed.
///
/// @param resolver The resolver to be used for our query.
///
/// @param host A string identifying a location. May be a descriptive name or
/// a numeric address string. If an empty string and the passive flag has been
/// specified, the resolved endpoints are suitable for local service binding.
/// If an empty string and passive is not specified, the resolved endpoints
/// will use the loopback address.
///
/// @param service A string identifying the requested service. This may be a
/// descriptive name or a numeric string corresponding to a port number. May
/// be an empty string, in which case all resolved endpoints will have a port
/// number of 0.
///
/// @param resolve_flags A set of flags that determine how name resolution
/// should be performed. The default flags are suitable for communication with
/// remote hosts. See the @ref resolver_base documentation for the set of
/// available flags.
///
/// @param token The @ref asio::completion_token that will be used to produce a
/// completion handler, which will be called when the connect operation completes.
/// Potential completion tokens include @ref asio::use_future, @ref asio::use_awaitable,
/// @ref asio::yield_context, or a function object with the correct completion
/// signature. The function signature of the completion handler must be:
/// @code
/// void handler(
///   // Result of operation. if the sequence is empty, set to
///   // asio::error::not_found. Otherwise, contains the
///   // error from the last connection attempt.
///   const error_code& error,
///
///   // On success, the successfully connected endpoint.
///   // Otherwise, a default-constructed endpoint.
///   const typename Protocol::endpoint& endpoint
/// );
/// @endcode
template <typename Protocol,
          typename Executor,
          asio::completion_token_for<void(error_code, typename Protocol::endpoint)> ComposedConnectToken =
              asio::default_completion_token_t<Executor>>
auto async_connect(asio::basic_socket<Protocol, Executor>& socket,
                   asio::ip::basic_resolver<Protocol>& resolver,
                   std::string_view host,
                   std::string_view service,
                   asio::ip::resolver_base::flags resolve_flags,
                   ComposedConnectToken&& token = asio::default_completion_token_t<Executor>())
{
  return asio::async_compose<ComposedConnectToken, void(error_code, typename Protocol::endpoint)>(
      detail::connect_op<Protocol, Executor>{socket, resolver, host, service, resolve_flags}, token, socket, resolver);
}

/// @}

ASIOEXT_NS_END

#if defined(ASIOEXT_HEADER_ONLY)
#  include "asioext/impl/connect.cpp"
#endif

#endif
