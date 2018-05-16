/// @copyright Copyright (c) 2018 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#include "asioext/socks/detail/protocol.hpp"

ASIOEXT_NS_BEGIN

namespace socks {
namespace detail {

std::size_t get_sgreet_packet_size(const auth_method* auth_methods,
                                   std::size_t num_auth_methods)
{
  if (num_auth_methods > 255)
    return 0;

  return num_auth_methods + 2;
}

std::size_t get_slogin_packet_size(const std::string& username,
                                   const std::string& password)
{
  const std::size_t username_size = username.size();
  const std::size_t password_size = password.size();
  if (username_size > 255 || password_size > 255)
    return 0;

  return username_size + password_size + 3;
}

std::size_t get_sexec_packet_size(command cmd,
                                  const asio::ip::tcp::endpoint& remote,
                                  const std::string& remote_host,
                                  uint16_t port)
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

void encode_sgreet_packet(const auth_method* auth_methods,
                          std::size_t num_auth_methods,
                          uint8_t* buf)
{
  *buf++ = 5;
  *buf++ = static_cast<uint8_t>(num_auth_methods);
  for (std::size_t i = 0; i != num_auth_methods; ++i)
    *buf++ = static_cast<uint8_t>(auth_methods[i]);
}

void encode_slogin_packet(const std::string& username,
                          const std::string& password,
                          uint8_t* buf)
{
  const std::size_t username_size = username.size();
  const std::size_t password_size = password.size();

  *buf++ = 1;

  *buf++ = static_cast<uint8_t>(username_size);
  std::memcpy(buf, username.data(), username_size);
  buf += username_size;

  *buf++ = static_cast<uint8_t>(password_size);
  std::memcpy(buf, password.data(), password_size);
  buf += password_size;
}

void encode_sexec_packet(command cmd,
                         const asio::ip::tcp::endpoint& remote,
                         const std::string& remote_host,
                         uint16_t port,
                         uint8_t* buf)
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

  *buf++ = 5;
  *buf++ = static_cast<uint8_t>(cmd);
  *buf++ = 0;

  if (!remote_host.empty()) {
    *buf++ = 3;

    *buf++ = static_cast<uint8_t>(remote_host_size);
    std::memcpy(buf, remote_host.data(), remote_host_size);
    buf += remote_host_size;

    *buf++ = static_cast<uint8_t>((port >> 8) & 0xff);
    *buf++ = static_cast<uint8_t>((port >> 0) & 0xff);
  } else {
    const uint16_t rport = remote.port();
    *buf++ = is_v4 ? 1 : 4;
    if (is_v4) {
      const asio::ip::address_v4::bytes_type addr =
          remote.address().to_v4().to_bytes();
      for (std::size_t i = 0; i != addr.size(); ++i)
        *buf++ = addr[i];
    } else {
      const asio::ip::address_v6::bytes_type addr =
          remote.address().to_v6().to_bytes();
      for (std::size_t i = 0; i != addr.size(); ++i)
        *buf++ = addr[i];
    }
    *buf++ = static_cast<uint8_t>((rport >> 8) & 0xff);
    *buf++ = static_cast<uint8_t>((rport >> 0) & 0xff);
  }
}

}
}

ASIOEXT_NS_END
