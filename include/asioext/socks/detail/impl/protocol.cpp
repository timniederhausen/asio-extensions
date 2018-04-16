/// @copyright Copyright (c) 2018 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#include "asioext/socks/detail/protocol.hpp"

ASIOEXT_NS_BEGIN

namespace socks {
namespace detail {

bool encode_sgreet_packet(const auth_method* auth_methods,
                          std::size_t num_auth_methods,
                          linear_buffer& out)
{
  if (num_auth_methods > 255)
    return false;

  uint8_t* buf = asio::buffer_cast<uint8_t*>(
      out.prepare(num_auth_methods + 2));
  buf[0] = 5;
  buf[1] = static_cast<uint8_t>(num_auth_methods);
  for (std::size_t i = 0; i != num_auth_methods; ++i)
    buf[i + 2] = static_cast<uint8_t>(auth_methods[i]);
  out.commit(num_auth_methods + 2);
  return true;
}

bool encode_slogin_packet(const std::string& username,
                          const std::string& password,
                          linear_buffer& out)
{
  const std::size_t username_size = username.size();
  const std::size_t password_size = password.size();
  if (username_size > 255 || password_size > 255)
    return false;

  uint8_t* buf = asio::buffer_cast<uint8_t*>(
      out.prepare(username_size + password_size + 3));
  buf[0] = 1;
  buf[1] = static_cast<uint8_t>(username_size);
  std::memcpy(&buf[2], username.data(), username_size);
  buf[2 + username_size] = static_cast<uint8_t>(password_size);
  std::memcpy(&buf[3 + username_size], password.data(), password_size);
  out.commit(username_size + password_size + 3);
  return true;
}

bool encode_sexec_packet(command cmd,
                         const asio::ip::tcp::endpoint& remote,
                         const std::string& remote_host,
                         uint16_t port,
                         linear_buffer& out)
{
  const std::size_t remote_host_size = remote_host.size();
  if (remote_host_size > 255)
    return false;

  const bool is_v4 = remote.protocol() == asio::ip::tcp::v4();

  std::size_t size = 6;
  if (!remote_host.empty())
    size += remote_host_size + 1;
  else if (is_v4)
    size += 4;
  else
    size += 16;

  uint8_t* buf = asio::buffer_cast<uint8_t*>(out.prepare(size));
  buf[0] = 5;
  buf[1] = static_cast<uint8_t>(cmd);
  buf[2] = 0;
  if (!remote_host.empty()) {
    buf[3] = 3;
    buf[4] = static_cast<uint8_t>(remote_host_size);
    std::memcpy(buf + 5, remote_host.data(), remote_host_size);
    buf[5 + remote_host_size] = static_cast<uint8_t>((port >> 8) & 0xff);
    buf[6 + remote_host_size] = static_cast<uint8_t>((port >> 0) & 0xff);
  } else {
    const uint16_t rport = remote.port();
    buf[3] = is_v4 ? 1 : 4;
    if (is_v4) {
      const asio::ip::address_v4::bytes_type addr =
          remote.address().to_v4().to_bytes();
      for (std::size_t i = 0; i != addr.size(); ++i)
        buf[4 + i] = addr[i];

      buf[4 + addr.size()] = static_cast<uint8_t>((rport >> 8) & 0xff);
      buf[5 + addr.size()] = static_cast<uint8_t>((rport >> 0) & 0xff);
    } else {
      const asio::ip::address_v6::bytes_type addr =
          remote.address().to_v6().to_bytes();
      for (std::size_t i = 0; i != addr.size(); ++i)
        buf[4 + i] = addr[i];

      buf[4 + addr.size()] = static_cast<uint8_t>((rport >> 8) & 0xff);
      buf[5 + addr.size()] = static_cast<uint8_t>((rport >> 0) & 0xff);
    }
  }
  out.commit(size);
  return true;
}

}
}

ASIOEXT_NS_END
