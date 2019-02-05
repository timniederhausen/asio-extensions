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

// SOCKS v4a

ASIOEXT_DECL std::size_t get_exec_packet_size(
    const asio::ip::tcp::endpoint& remote,
    const std::string& remote_host,
    const std::string& user_id) ASIOEXT_NOEXCEPT;

ASIOEXT_DECL void encode_exec_packet(
    command cmd,
    const asio::ip::tcp::endpoint& remote,
    const std::string& remote_host,
    uint16_t port,
    const std::string& user_id,
    uint8_t* out) ASIOEXT_NOEXCEPT;

// SOCKS v5

ASIOEXT_DECL std::size_t get_greet_packet_size(
    const auth_method* auth_methods,
    std::size_t num_auth_methods) ASIOEXT_NOEXCEPT;

ASIOEXT_DECL std::size_t get_login_packet_size(
    const std::string& username,
    const std::string& password) ASIOEXT_NOEXCEPT;

ASIOEXT_DECL std::size_t get_exec_packet_size(
    command cmd,
    const asio::ip::tcp::endpoint& remote,
    const std::string& remote_host, uint16_t port) ASIOEXT_NOEXCEPT;

ASIOEXT_DECL void encode_greet_packet(
    const auth_method* auth_methods,
    std::size_t num_auth_methods,
    uint8_t* out) ASIOEXT_NOEXCEPT;

ASIOEXT_DECL void encode_login_packet(
    const std::string& username,
    const std::string& password,
    uint8_t* out) ASIOEXT_NOEXCEPT;

ASIOEXT_DECL void encode_exec_packet(
    command cmd,
    const asio::ip::tcp::endpoint& remote,
    const std::string& remote_host,
    uint16_t port,
    uint8_t* out) ASIOEXT_NOEXCEPT;

}
}

ASIOEXT_NS_END

#if defined(ASIOEXT_HEADER_ONLY)
# include "asioext/socks/detail/impl/protocol.cpp"
#endif

#endif
