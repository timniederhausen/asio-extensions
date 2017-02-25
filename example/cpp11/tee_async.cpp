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
#include <asio/use_future.hpp>

#include <future>
#include <iostream>
#include <cstdio>

using file_handles = std::vector<asioext::file>;

bool tee_file(asioext::file_handle source, file_handles& destinations)
{
  char buffer[16 * 1024];
  std::error_code ec;

  std::vector<std::future<std::size_t>> futures(destinations.size());

  while (true) {
    const std::size_t bytes_read = source.read_some(asio::buffer(buffer), ec);

    // Handle the expected errors here. (We could also use the throwing version
    // and catch & re-throw here, but this'd be overkill.)
    if (ec) {
      if (ec == asio::error::eof || ec == asio::error::broken_pipe)
        break;
      else
        throw std::system_error(ec);
    }

    // Start |n| asynchronous write operations.
    for (std::size_t i = 0, n = destinations.size(); i != n; ++i)
      futures[i] = asio::async_write(destinations[i],
                                     asio::buffer(buffer, bytes_read),
                                     asio::use_future);

    // Wait for |n| operations to complete. Because our thread-pool has |n|
    // threads, these writes will (probably) execute concurrently.
    // (If we didn't have to re-use the buffer, we could skip waiting for
    // completion.)
    for (std::size_t i = 0, n = destinations.size(); i != n; ++i)
      futures[i].get();
  }

  return true;
}

int main(int argc, const char* argv[])
{
  if (argc < 2) {
    std::cerr << "usage: tee files..." << std::endl;
    return 1;
  }

  asio::io_service io_service;

  // Make a service with |argc - 1| threads, so all writes
  // can happen in parallel.
  auto svc = new asioext::thread_pool_file_service(io_service, argc - 1);
  asio::add_service(io_service, svc);

  file_handles files;
  files.reserve(argc - 1);

  std::error_code ec;
  for (int i = 1; i != argc; ++i) {
    files.emplace_back(asioext::file(io_service, argv[i],
                                     asioext::create_always |
                                     asioext::access_write,
                                     ec));
    if (ec) {
      std::cerr << "Failed to open " << argv[i] << " with " << ec << '\n';
      return 1;
    }
  }

  try {
    std::thread t;
    bool result = false;

    {
      asio::io_service::work w(io_service);
      t = std::thread([&io_service]() { io_service.run(); });
      result = tee_file(asioext::get_stdin(), files);
    }

    t.join();
    return result ? 0 : 1;
  } catch (std::exception& e) {
    std::cerr << "fatal: copying data failed with " << e.what() << '\n';
    return 1;
  }
}
