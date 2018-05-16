/// @copyright Copyright (c) 2017 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#include <asioext/file.hpp>
#include <asioext/thread_pool_file_service.hpp>
#include <asioext/open_flags.hpp>
#include <asioext/standard_streams.hpp>

#include <asio/write.hpp>
#include <asio/io_service.hpp>

#include <iostream>
#include <cstdio>

using file_handles = std::vector<asioext::file>;

class tee_file
{
public:
  tee_file(asioext::file& source, file_handles& destinations)
    : source_(source)
    , destinations_(destinations)
    , completed_(0)
  {
    // ctor
  }

  void run();

private:
  asioext::file& source_;
  file_handles& destinations_;
  char buffer_[16 * 1024];
  std::size_t completed_;
};

void tee_file::run()
{
  source_.async_read_some(asio::buffer(buffer_),
                          [this] (const std::error_code& ec,
                                  std::size_t bytes_read) {
    if (ec) {
      if (ec == asio::error::eof || ec == asio::error::broken_pipe)
        return;
      else
        throw std::system_error(ec);
    }

    auto write_handler = [this] (const std::error_code& ec,
                                 std::size_t /*not needed*/) {
      if (ec)
        throw std::system_error(ec);

      // Order of completion isn't important.
      if (++completed_ == destinations_.size())
        run();
    };

    // Start |n| asynchronous write operations.
    // Because our thread-pool has |n|
    // threads, these writes will (probably) execute concurrently.
    completed_ = 0;
    for (std::size_t i = 0, n = destinations_.size(); i != n; ++i)
      asio::async_write(destinations_[i],
                        asio::buffer(buffer_, bytes_read),
                        write_handler);
  });
}

int main(int argc, const char* argv[])
{
  if (argc < 2) {
    std::cerr << "usage: tee files..." << std::endl;
    return 1;
  }

  asio::io_service io_service;

  // Make a service with |argc - 1| threads, so all writes
  // *can* happen in parallel.
  auto svc = new asioext::thread_pool_file_service(io_service, argc - 1);
  asio::add_service(io_service, svc);

  file_handles files;
  files.reserve(argc - 1);

  std::error_code ec;
  for (int i = 1; i != argc; ++i) {
    files.emplace_back(asioext::file(io_service, argv[i],
                                     asioext::open_flags::create_always |
                                     asioext::open_flags::access_write, ec));
    if (ec) {
      std::cerr << "Failed to open " << argv[i] << " with " << ec << '\n';
      return 1;
    }
  }

  try {
    asioext::file source(io_service, asioext::get_stdin());

    tee_file op(source, files);
    op.run();

    io_service.run();
    return 0;
  } catch (std::exception& e) {
    std::cerr << "fatal: copying data failed with " << e.what() << '\n';
    return 1;
  }
}
