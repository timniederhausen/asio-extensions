#include "test_file_rm_guard.hpp"

#include "asioext/open_flags.hpp"
#include "asioext/basic_file.hpp"
#include "asioext/thread_pool_file_service.hpp"

#if defined(ASIOEXT_USE_BOOST_ASIO)
# include <boost/asio/write.hpp>
# include <boost/asio/read.hpp>
#else
# include <asio/write.hpp>
# include <asio/read.hpp>
#endif

#include <boost/test/unit_test.hpp>
#include <boost/mpl/list.hpp>

ASIOEXT_NS_BEGIN

BOOST_AUTO_TEST_SUITE(asioext_basic_file)

// BOOST_AUTO_TEST_SUITE() gives us a unique NS, so we don't need to
// prefix our variables.

static const char empty_filename[] = "asioext_basicfile_empty";
static const wchar_t empty_filenamew[] = L"asioext_basicfile_empty2";

static const char test_filename[] = "asioext_basicfile_test";
static const wchar_t test_filenamew[] = L"asioext_basicfile_test";
static const char test_data[] = "hello world!";
static const std::size_t test_data_size = sizeof(test_data) - 1;

typedef boost::mpl::list<asioext::thread_pool_file_service> service_types;

BOOST_AUTO_TEST_CASE_TEMPLATE(empty, FileService, service_types)
{
  asio::io_service io_service;
  asioext::basic_file<FileService> file(io_service);
  BOOST_CHECK(!file.is_open());
  BOOST_CHECK_NO_THROW(file.close());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(constructor, FileService, service_types)
{
  test_file_rm_guard rguard1(empty_filename);

  asio::io_service io_service;
  asioext::error_code ec;
  asioext::basic_file<FileService> f1(io_service, "nosuchfile",
                                      access_read | open_existing, ec);
  BOOST_REQUIRE(ec);
  asioext::basic_file<FileService> f2(io_service, empty_filename,
                                      access_write | open_always, ec);
  BOOST_REQUIRE(!ec);
#if defined(ASIOEXT_WINDOWS)
  test_file_rm_guard rguard2(empty_filenamew);
  asioext::basic_file<FileService> f3(io_service, L"nosuchfile",
                                      access_read | open_existing, ec);
  BOOST_REQUIRE(ec);
  asioext::basic_file<FileService> f4(io_service, empty_filenamew,
                                      access_write | open_always, ec);
  BOOST_REQUIRE(!ec);
#endif
}

BOOST_AUTO_TEST_CASE_TEMPLATE(open, FileService, service_types)
{
  test_file_rm_guard rguard1(empty_filename);
#if defined(ASIOEXT_WINDOWS)
  test_file_rm_guard rguard2(empty_filenamew);
#endif

  asio::io_service io_service;
  asioext::basic_file<FileService> file(io_service);

  asioext::error_code ec;
  file.open("nosuchfile", access_read | open_existing, ec);

  BOOST_REQUIRE(!!ec);
  BOOST_CHECK(!file.is_open());
  BOOST_CHECK_NO_THROW(file.close());

#if defined(ASIOEXT_WINDOWS)
  file.open(L"nosuchfile", access_read | open_existing, ec);

  BOOST_REQUIRE(!!ec);
  BOOST_CHECK(!file.is_open());
  BOOST_CHECK_NO_THROW(file.close());
#endif

  file.open(empty_filename, access_write | open_always, ec);

  BOOST_REQUIRE(!ec);
  BOOST_CHECK(file.is_open());

  BOOST_CHECK_NO_THROW(file.close());
  BOOST_CHECK(!file.is_open());

#if defined(ASIOEXT_WINDOWS)
  file.open(empty_filenamew, access_write | open_always, ec);
  BOOST_REQUIRE(!ec);
  BOOST_CHECK(file.is_open());
#endif
}

BOOST_AUTO_TEST_CASE_TEMPLATE(read_write, FileService, service_types)
{
  test_file_rm_guard rguard1(test_filename);

  asio::io_service io_service;
  asioext::basic_file<FileService> file(io_service);

  asioext::error_code ec;
  file.open(test_filename, access_write | create_always, ec);
  BOOST_REQUIRE(!ec);

  BOOST_REQUIRE_EQUAL(0, asio::write(file, asio::buffer(test_data, 0)));
  BOOST_REQUIRE_EQUAL(test_data_size,
                      asio::write(file, asio::buffer(test_data,
                                                     test_data_size)));

  BOOST_REQUIRE_NO_THROW(file.close());

  file.open(test_filename, access_read | open_existing, ec);
  BOOST_REQUIRE(!ec);

  char buffer[128];
  BOOST_REQUIRE_EQUAL(test_data_size,
                      asio::read(file, asio::buffer(buffer, test_data_size)));
  BOOST_REQUIRE_EQUAL(0, std::memcmp(test_data, buffer, test_data_size));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(position_and_size, FileService, service_types)
{
  test_file_rm_guard rguard1(test_filename);

  asio::io_service io_service;
  asioext::basic_file<FileService> file(io_service);

  asioext::error_code ec;
  file.open(test_filename, access_write | create_always, ec);
  BOOST_REQUIRE(!ec);

  BOOST_REQUIRE_EQUAL(0, file.position());
  BOOST_REQUIRE_EQUAL(test_data_size,
                      asio::write(file, asio::buffer(test_data,
                                                     test_data_size)));
  BOOST_REQUIRE_EQUAL(test_data_size, file.position());
  BOOST_REQUIRE_EQUAL(test_data_size, file.size());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(seek, FileService, service_types)
{
  test_file_rm_guard rguard1(test_filename);

  asio::io_service io_service;
  asioext::basic_file<FileService> file(io_service);

  asioext::error_code ec;
  file.open(test_filename, access_write | create_always, ec);
  BOOST_REQUIRE(!ec);

  BOOST_REQUIRE_EQUAL(0, file.seek(asioext::file_handle::from_current, 0));
  BOOST_REQUIRE_EQUAL(10, file.seek(asioext::file_handle::from_current, 10));
  BOOST_REQUIRE_EQUAL(10, file.position());
  BOOST_REQUIRE_EQUAL(20, file.seek(asioext::file_handle::from_current, 10));
  BOOST_REQUIRE_EQUAL(20, file.position());
  BOOST_REQUIRE_EQUAL(10, file.seek(asioext::file_handle::from_current, -10));

  char buffer[10] = {0, };
  BOOST_REQUIRE_EQUAL(10, asio::write(file, asio::buffer(buffer)));

  // size is now 20
  BOOST_REQUIRE_EQUAL(10, file.seek(asioext::file_handle::from_end, -10));
}

template <class FileService>
struct write_handler
{
  write_handler(asioext::basic_file<FileService>& file)
    : file_(file)
    , state_(0)
  {
    // ctor
  }

  void operator()(const error_code& ec, std::size_t bytes_transferred)
  {
    BOOST_REQUIRE(!ec);
    switch (state_) {
      case 0: {
        state_ = 1;
        BOOST_REQUIRE_EQUAL(0, bytes_transferred);
        BOOST_REQUIRE_NO_THROW(asio::async_write(file_,
            asio::buffer(test_data, test_data_size),
            ASIOEXT_MOVE_CAST(write_handler)(*this)));
        break;
      }
      case 1: {
        BOOST_REQUIRE_EQUAL(test_data_size, bytes_transferred);
        break;
      }
    }
  }

  asioext::basic_file<FileService>& file_;
  int state_;
};

template <class FileService>
struct read_handler
{
  read_handler(asioext::basic_file<FileService>& file,
               const asio::mutable_buffer& buffer)
    : file_(file)
    , buffer_(buffer)
    , state_(0)
  {
    // ctor
  }

  void operator()(const error_code& ec, std::size_t bytes_transferred)
  {
    BOOST_REQUIRE_MESSAGE(!ec,
                          "Operation " << state_ << " failed with: " << ec);
    switch (state_) {
      case 0: {
        state_ = 1;
        BOOST_REQUIRE_EQUAL(0, bytes_transferred);
        BOOST_REQUIRE_NO_THROW(asio::async_read(file_,
            asio::buffer(buffer_),
            ASIOEXT_MOVE_CAST(read_handler)(*this)));
        break;
      }
      case 1: {
        BOOST_REQUIRE_EQUAL(test_data_size, bytes_transferred);
        BOOST_REQUIRE_EQUAL(0, std::memcmp(test_data,
                                           asio::buffer_cast<char*>(buffer_),
                                           test_data_size));
        break;
      }
    }
  }

  asioext::basic_file<FileService>& file_;
  asio::mutable_buffer buffer_;
  int state_;
};

BOOST_AUTO_TEST_CASE_TEMPLATE(async_read_write, FileService, service_types)
{
  test_file_rm_guard rguard1(test_filename);

  asio::io_service io_service;
  asioext::basic_file<FileService> file(io_service);

  asioext::error_code ec;
  file.open(test_filename, access_write | create_always, ec);
  BOOST_REQUIRE(!ec);

  BOOST_REQUIRE_NO_THROW(asio::async_write(file, asio::buffer(test_data, 0),
                                           write_handler<FileService>(file)));

  // Run the just enqueued operations.
  io_service.run();
  io_service.reset();

  BOOST_REQUIRE_NO_THROW(file.close());

  file.open(test_filename, access_read | open_existing, ec);
  BOOST_REQUIRE(!ec);

  char buffer[128];
  BOOST_REQUIRE_NO_THROW(asio::async_read(
      file, asio::buffer(buffer, 0),
      read_handler<FileService>(file,
                                asio::mutable_buffer(buffer, test_data_size))
  ));

  // Run the just enqueued operations.
  io_service.run();
  io_service.reset();
}

template <class FileService>
struct write_cancel_handler
{
  void operator()(const error_code& ec, std::size_t bytes_transferred)
  {
    BOOST_REQUIRE_EQUAL(ec, asio::error::operation_aborted);
  }
};

template <class FileService>
struct cancel_handler
{
  cancel_handler(asioext::basic_file<FileService>& file)
    : file_(file)
    , work_(file.get_io_service())
  {
    // ctor
  }

  void operator()()
  {
    asio::async_write(
        file_, asio::buffer(test_data, test_data_size),
        write_cancel_handler<FileService>());

    // Cancel the just enqueued operation. Since we have only
    // one thread (which is currently executing this function),
    // we can be sure, that the cancellation must succeed before the
    // operation starts.
    file_.cancel();
  }

  asioext::basic_file<FileService>& file_;
  asio::io_service::work work_;
};

BOOST_AUTO_TEST_CASE(async_read_write_cancel)
{
  typedef thread_pool_file_service FileService;

  test_file_rm_guard rguard1(test_filename);

  asio::io_service io_service;

  FileService* svc = new FileService(io_service, 1);
  asio::add_service(io_service, svc);

  asioext::basic_file<FileService> file(io_service);

  asioext::error_code ec;
  file.open(test_filename, access_write | create_always, ec);
  BOOST_REQUIRE(!ec);
  BOOST_REQUIRE(file.is_open());

  svc->get_pool_io_service().post(cancel_handler<FileService>(file));

  // Run the just enqueued operations.
  io_service.run();
  io_service.reset();
}

BOOST_AUTO_TEST_SUITE_END()

ASIOEXT_NS_END
