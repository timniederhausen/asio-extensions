/// @copyright Copyright (c) 2018 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_SOCKS_DETAIL_PROTOCOL_HPP
#define ASIOEXT_SOCKS_DETAIL_PROTOCOL_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include "asioext/socks/constants.hpp"
#include "asioext/linear_buffer.hpp"

#if defined(ASIOEXT_USE_BOOST_ASIO)
# include <boost/asio/ip/tcp.hpp>
#else
# include <asio/ip/tcp.hpp>
#endif

ASIOEXT_NS_BEGIN

namespace socks {
namespace detail {

ASIOEXT_DECL bool encode_sgreet_packet(
    const auth_method* auth_methods,
    std::size_t num_auth_methods,
    linear_buffer& out);

ASIOEXT_DECL bool encode_slogin_packet(
    const std::string& username,
    const std::string& password,
    linear_buffer& out);

ASIOEXT_DECL bool encode_sexec_packet(
    command cmd,
    const asio::ip::tcp::endpoint& remote,
    const std::string& remote_host,
    uint16_t port,
    linear_buffer& out);

}
}

ASIOEXT_NS_END

#if defined(ASIOEXT_HEADER_ONLY)
# include "asioext/socks/detail/impl/protocol.cpp"
#endif

#endif
