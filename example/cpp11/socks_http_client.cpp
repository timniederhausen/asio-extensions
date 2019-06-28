#include <asioext/socks/client.hpp>
#include <asioext/connect.hpp>
#include <asioext/compose.hpp>

#include <asio/ip/tcp.hpp>
#include <asio/io_service.hpp>
#include <asio/streambuf.hpp>
#include <asio/read.hpp>
#include <asio/read_until.hpp>
#include <asio/write.hpp>
#include <asio/coroutine.hpp>

#include <iostream>
#include <istream>
#include <ostream>
#include <string>

namespace {

// This helper function expects |username|, |password| and |buffer| to
// live at least as long as the initiated asynchronous operation.
template <typename Socket, typename Handler>
asioext::async_result_t<Handler, void (asioext::error_code, std::size_t)>
async_socks_login(Socket& socket, const std::string& username,
                  const std::string& password, asioext::linear_buffer& buffer,
                  Handler&& handler)
{
  auto op = [
    &socket, &buffer, &username, &password,
    c = asio::coroutine()
  ] (auto& self, asioext::error_code ec = asioext::error_code(),
     asioext::socks::auth_method chosen_method =
        asioext::socks::auth_method::none) mutable
  {
    if (ec) {
      self.complete(ec);
      return;
    }

    static const asioext::socks::auth_method auth_methods[] = {
      asioext::socks::auth_method::none,
      asioext::socks::auth_method::username_password,
    };
    std::size_t auth_method_count = 2;
    if (username.empty())
      auth_method_count = 1;

    ASIO_CORO_REENTER (c) {
      ASIO_CORO_YIELD asioext::socks::async_greet(
          socket, auth_methods, auth_method_count,
          asioext::dynamic_buffer(buffer), std::move(self));

      if (chosen_method == asioext::socks::auth_method::none) {
        self.complete(asioext::socks::error::no_acceptable_auth_method);
        return;
      }

      ASIO_CORO_YIELD asioext::socks::async_login(
          socket, username, password, asioext::dynamic_buffer(buffer),
          std::move(self));
      self.complete(ec);
    }
  };
  return asioext::async_compose<Handler, void (asioext::error_code, std::size_t)>(
      std::move(op), handler);
}

}

class client
{
public:
  client(asio::io_service& io_service,
         const std::string& socks_server, const std::string& socks_port,
         const std::string& server, const std::string& path)
    : resolver_(io_service)
    , socket_(io_service)
    , server_(server)
  {
    // Form the request. We specify the "Connection: close" header so that the
    // server will close the socket after transmitting the response. This will
    // allow us to treat all data up until the EOF as the content.
    std::ostream request_stream(&request_);
    request_stream << "GET " << path << " HTTP/1.0\r\n";
    request_stream << "Host: " << server << "\r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Connection: close\r\n\r\n";

    asioext::async_connect(socket_, resolver_,
                           asio::ip::tcp::resolver::query(socks_server,
                                                          socks_port),
                           [this] (asioext::error_code ec,
                                   asio::ip::tcp::resolver::iterator it) {
      if (ec) {
        std::cout << "Error: " << ec << "\n";
        return;
      }

      async_socks_login(socket_, username_, password_,
                        socks_buffer_,
                        [this] (asioext::error_code ec) { on_logged_in(ec); });
    });
  }

private:
  void on_logged_in(asioext::error_code ec)
  {
    if (ec) {
      std::cout << "Error: " << ec << "\n";
      return;
    }

    asioext::socks::async_execute(
        socket_, asioext::socks::command::connect,
        server_, 80, asioext::dynamic_buffer(socks_buffer_),
        [this] (asioext::error_code ec) { handle_target_connect(ec); });
  }

  void handle_target_connect(asio::error_code ec)
  {
    if (ec) {
      std::cout << "Error: " << ec << "\n";
      return;
    }

    asio::async_write(socket_, request_,
                      [this] (asioext::error_code ec,
                              std::size_t) {
      if (ec) {
        std::cout << "Error: " << ec << "\n";
        return;
      }
      asio::async_read_until(socket_, response_, "\r\n",
                             [this] (asioext::error_code ec,
                                     std::size_t) {
        handle_read_status_line(ec);
      });
    });
  }

  void handle_read_status_line(asio::error_code ec)
  {
    if (ec) {
      std::cout << "Error: " << ec << "\n";
      return;
    }

    std::string http_version;
    unsigned int status_code;
    std::string status_message;

    // Check that response is OK.
    std::istream response_stream(&response_);
    response_stream >> http_version;
    response_stream >> status_code;
    std::getline(response_stream, status_message);

    if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
      std::cout << "Invalid response\n";
      return;
    }

    if (status_code != 200) {
      std::cout << "Response returned with status code ";
      std::cout << status_code << "\n";
      return;
    }

    // Read the response headers, which are terminated by a blank line.
    asio::async_read_until(socket_, response_, "\r\n\r\n",
                           [this] (asioext::error_code ec,
                                   std::size_t) {
      handle_read_headers(ec);
    });
  }

  void handle_read_headers(asio::error_code ec)
  {
    if (ec) {
      std::cout << "Error: " << ec << "\n";
      return;
    }

    // Process the response headers.
    std::istream response_stream(&response_);
    std::string header;
    while (std::getline(response_stream, header) && header != "\r")
      std::cout << header << "\n";
    std::cout << "\n";

    // Write whatever content we already have to output.
    if (response_.size() > 0)
      std::cout << &response_;

    // Start reading remaining data until EOF.
    asio::async_read(socket_, response_,
                     asio::transfer_at_least(1),
                     [this] (asioext::error_code ec, std::size_t) {
      handle_read_content(ec);
    });
  }

  void handle_read_content(asio::error_code ec)
  {
    if (ec) {
      std::cout << "Error: " << ec << "\n";
      return;
    }

    // Write all of the data that has been read so far.
    std::cout << &response_;

    // Continue reading remaining data until EOF.
    asio::async_read(socket_, response_,
                     asio::transfer_at_least(1),
                     [this] (asioext::error_code ec, std::size_t) {
      handle_read_content(ec);
    });
  }

  std::string server_;
  std::string username_{"foo"};
  std::string password_{"bar"};

  asio::ip::tcp::resolver resolver_;
  asio::ip::tcp::socket socket_;
  asioext::linear_buffer socks_buffer_;
  asio::streambuf request_;
  asio::streambuf response_;
};

int main(int argc, char* argv[])
{
  try {
    if (argc != 5) {
      std::cout << "Usage: async_socks_http_get <socks_server> <socks_port> <server> <path>\n";
      std::cout << "Example:\n";
      std::cout << "  async_socks_http_get 127.0.0.1 50000 www.boost.org /LICENSE_1_0.txt\n";
      return 1;
    }

    asio::io_service io_service;
    client c(io_service, argv[1], argv[2], argv[3], argv[4]);
    io_service.run();
    return 0;
  } catch (std::exception& e) {
    std::cout << "Exception: " << e.what() << "\n";
    return 1;
  }
}
