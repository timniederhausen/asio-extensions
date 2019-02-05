/// @copyright Copyright (c) 2018 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#include "asioext/socks/detail/protocol.hpp"

ASIOEXT_NS_BEGIN

namespace socks {
namespace detail {

// SOCKS v4a

std::size_t get_exec_packet_size(const asio::ip::tcp::endpoint& remote,
                                 const std::string& remote_host,
                                 const std::string& user_id) ASIOEXT_NOEXCEPT
{
  if (remote_host.empty() && remote.protocol() != asio::ip::tcp::v4())
    return 0;

  std::size_t size = 1 + 1 + 2 + 4;
  size += user_id.size() + 1;

  if (!remote_host.empty())
    size += remote_host.size() + 1;

  return size;
}

void encode_exec_packet(command cmd,
                        const asio::ip::tcp::endpoint& remote,
                        const std::string& remote_host, uint16_t port,
                        const std::string& user_id,
                        uint8_t* out) ASIOEXT_NOEXCEPT
{
  *out++ = 4; // SOCKS version

  *out++ = static_cast<uint8_t>(cmd);

  *out++ = static_cast<uint8_t>((port >> 8) & 0xff);
  *out++ = static_cast<uint8_t>((port >> 0) & 0xff);

  if (remote_host.empty()) {
    const asio::ip::address_v4::bytes_type addr =
        remote.address().to_v4().to_bytes();
    for (std::size_t i = 0; i != addr.size(); ++i)
      *out++ = addr[i];
  } else {
    // 0.0.0.1 - an invalid IP
    *out++ = 0;
    *out++ = 0;
    *out++ = 0;
    *out++ = 1;
  }

  std::memcpy(out, user_id.data(), user_id.size());
  out += user_id.size();
  *out++ = '\0';

  if (!remote_host.empty()) {
    std::memcpy(out, remote_host.data(), remote_host.size());
    out += remote_host.size();
    *out++ = '\0';
  }
}

// SOCKS v5

std::size_t get_greet_packet_size(const auth_method* auth_methods,
                                  std::size_t num_auth_methods) ASIOEXT_NOEXCEPT
{
  if (num_auth_methods > 255)
    return 0;

  return num_auth_methods + 2;
}

std::size_t get_login_packet_size(const std::string& username,
                                  const std::string& password) ASIOEXT_NOEXCEPT
{
  const std::size_t username_size = username.size();
  const std::size_t password_size = password.size();
  if (username_size > 255 || password_size > 255)
    return 0;

  return username_size + password_size + 3;
}

std::size_t get_exec_packet_size(command cmd,
                                 const asio::ip::tcp::endpoint& remote,
                                 const std::string& remote_host,
                                 uint16_t port) ASIOEXT_NOEXCEPT
{
  const std::size_t remote_host_size = remote_host.size();
  if (remote_host_size > 255)
    return 0;

  const bool is_v4 = remote.protocol() == asio::ip::tcp::v4();

  std::size_t size = 6;
  if (!remote_host.empty())
    size += remote_host_size + 1;
  else if (is_v4)
    size += 4;
  else
    size += 16;

  return size;
}

void encode_greet_packet(const auth_method* auth_methods,
                         std::size_t num_auth_methods,
                         uint8_t* out) ASIOEXT_NOEXCEPT
{
  *out++ = 5; // SOCKS version
  *out++ = static_cast<uint8_t>(num_auth_methods);
  for (std::size_t i = 0; i != num_auth_methods; ++i)
    *out++ = static_cast<uint8_t>(auth_methods[i]);
}

void encode_login_packet(const std::string& username,
                         const std::string& password,
                         uint8_t* out) ASIOEXT_NOEXCEPT
{
  const std::size_t username_size = username.size();
  const std::size_t password_size = password.size();

  *out++ = 1; // Authentication version

  *out++ = static_cast<uint8_t>(username_size);
  std::memcpy(out, username.data(), username_size);
  out += username_size;

  *out++ = static_cast<uint8_t>(password_size);
  std::memcpy(out, password.data(), password_size);
  out += password_size;
}

void encode_exec_packet(command cmd,
                        const asio::ip::tcp::endpoint& remote,
                        const std::string& remote_host,
                        uint16_t port,
                        uint8_t* out) ASIOEXT_NOEXCEPT
{
  const std::size_t remote_host_size = remote_host.size();

  const bool is_v4 = remote.protocol() == asio::ip::tcp::v4();

  std::size_t size = 6;
  if (!remote_host.empty())
    size += remote_host_size + 1;
  else if (is_v4)
    size += 4;
  else
    size += 16;

  *out++ = 5; // SOCKS version
  *out++ = static_cast<uint8_t>(cmd);
  *out++ = 0;

  if (!remote_host.empty()) {
    *out++ = 3;

    *out++ = static_cast<uint8_t>(remote_host_size);
    std::memcpy(out, remote_host.data(), remote_host_size);
    out += remote_host_size;

    *out++ = static_cast<uint8_t>((port >> 8) & 0xff);
    *out++ = static_cast<uint8_t>((port >> 0) & 0xff);
  } else {
    const uint16_t rport = remote.port();
    *out++ = is_v4 ? 1 : 4;
    if (is_v4) {
      const asio::ip::address_v4::bytes_type addr =
          remote.address().to_v4().to_bytes();
      for (std::size_t i = 0; i != addr.size(); ++i)
        *out++ = addr[i];
    } else {
      const asio::ip::address_v6::bytes_type addr =
          remote.address().to_v6().to_bytes();
      for (std::size_t i = 0; i != addr.size(); ++i)
        *out++ = addr[i];
    }
    *out++ = static_cast<uint8_t>((rport >> 8) & 0xff);
    *out++ = static_cast<uint8_t>((rport >> 0) & 0xff);
  }
}

}
}

ASIOEXT_NS_END
