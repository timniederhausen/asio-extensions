#include "test_file_rm_guard.hpp"

#include "asioext/unique_file_handle.hpp"
#include "asioext/open.hpp"

#if defined(ASIOEXT_USE_BOOST_ASIO)
# include <boost/asio/write.hpp>
# include <boost/asio/read.hpp>
#else
# include <asio/write.hpp>
# include <asio/read.hpp>
#endif

#include <boost/test/unit_test.hpp>

#include <thread>

ASIOEXT_NS_BEGIN

BOOST_AUTO_TEST_SUITE(asioext_file_handle)

// BOOST_AUTO_TEST_SUITE() gives us a unique NS, so we don't need to
// prefix our variables.

static const char* empty_filename = "asioext_scopedfilehandle_empty";
static const wchar_t* empty_filenamew = L"asioext_scopedfilehandle_empty2";

static const char* test_filename = "asioext_scopedfilehandle_test";
static const wchar_t* test_filenamew = L"asioext_scopedfilehandle_test";
static const char test_data[] = "hello world!";
static const std::size_t test_data_size = sizeof(test_data) - 1;

BOOST_AUTO_TEST_CASE(empty)
{
  asioext::unique_file_handle fh;
  BOOST_CHECK(!fh.is_open());
  BOOST_CHECK_NO_THROW(fh.close());
}

BOOST_AUTO_TEST_CASE(ownership_transfer)
{
  test_file_rm_guard rguard1(empty_filename);

  asioext::error_code ec;
  auto fh = asioext::open(empty_filename,
                          asioext::open_flags::access_write |
                          asioext::open_flags::create_always,
                          asioext::file_perms::create_default,
                          asioext::file_attrs::none, ec);
  BOOST_REQUIRE_MESSAGE(!ec, "ec: " << ec);

  asioext::file_handle h = fh.release();

  asioext::unique_file_handle fh2;
  fh2.reset(h, ec);
  BOOST_REQUIRE_MESSAGE(!ec, "ec: " << ec);
  BOOST_CHECK(!fh.is_open());
  BOOST_CHECK(fh2.is_open());
  BOOST_REQUIRE_EQUAL(fh2.get().native_handle(), h.native_handle());

  asioext::unique_file_handle fh3(fh2.release());
  BOOST_CHECK(!fh2.is_open());
  BOOST_CHECK(fh3.is_open());
  BOOST_REQUIRE_EQUAL(fh3.get().native_handle(), h.native_handle());
}

BOOST_AUTO_TEST_CASE(read_write)
{
  test_file_rm_guard rguard1(test_filename);

  asioext::unique_file_handle fh;

  asioext::error_code ec;
  fh = asioext::open(test_filename,
                     asioext::open_flags::access_write |
                     asioext::open_flags::create_always,
                     asioext::file_perms::create_default,
                     asioext::file_attrs::none, ec);
  BOOST_REQUIRE_MESSAGE(!ec, "ec: " << ec);

  BOOST_REQUIRE_EQUAL(0, asio::write(fh, asio::buffer(test_data, 0)));
  BOOST_REQUIRE_EQUAL(test_data_size,
                      asio::write(fh, asio::buffer(test_data,
                                                   test_data_size)));

  asio::const_buffers_1 empty_buffer1(nullptr, 0);
  BOOST_REQUIRE_EQUAL(0, fh.write_some_at(0, empty_buffer1));
  BOOST_REQUIRE_EQUAL(0, fh.write_some_at(1, empty_buffer1));

  BOOST_REQUIRE_NO_THROW(fh.close());

  fh = asioext::open(test_filename,
                     asioext::open_flags::access_read |
                     asioext::open_flags::open_existing,
                     asioext::file_perms::create_default,
                     asioext::file_attrs::none, ec);
  BOOST_REQUIRE_MESSAGE(!ec, "ec: " << ec);

  asio::mutable_buffers_1 empty_buffer2(nullptr, 0);
  BOOST_REQUIRE_EQUAL(0, fh.read_some_at(0, empty_buffer2));
  BOOST_REQUIRE_EQUAL(0, fh.read_some_at(1, empty_buffer2));

  char buffer[128];
  BOOST_REQUIRE_EQUAL(0,
                      asio::read(fh, asio::buffer(buffer, 0)));
  BOOST_REQUIRE_EQUAL(test_data_size,
                      asio::read(fh, asio::buffer(buffer, test_data_size)));
  BOOST_REQUIRE_EQUAL(0, std::memcmp(test_data, buffer, test_data_size));
}

BOOST_AUTO_TEST_CASE(position)
{
  test_file_rm_guard rguard1(test_filename);

  asioext::unique_file_handle fh;

  asioext::error_code ec;
  fh = asioext::open(test_filename,
                     asioext::open_flags::access_write |
                     asioext::open_flags::create_always,
                     asioext::file_perms::create_default,
                     asioext::file_attrs::none, ec);
  BOOST_REQUIRE_MESSAGE(!ec, "ec: " << ec);

  BOOST_REQUIRE_EQUAL(0, fh.position());
  BOOST_REQUIRE_EQUAL(test_data_size,
                      asio::write(fh, asio::buffer(test_data,
                                                   test_data_size)));
  BOOST_REQUIRE_EQUAL(test_data_size, fh.position());
}

BOOST_AUTO_TEST_CASE(seek)
{
  test_file_rm_guard rguard1(test_filename);

  asioext::unique_file_handle fh;

  asioext::error_code ec;
  fh = open(test_filename,
            asioext::open_flags::access_write |
            asioext::open_flags::create_always,
            asioext::file_perms::create_default,
            asioext::file_attrs::none, ec);
  BOOST_REQUIRE_MESSAGE(!ec, "ec: " << ec);

  BOOST_REQUIRE_EQUAL(0, fh.seek(asioext::seek_origin::from_current, 0));
  BOOST_REQUIRE_EQUAL(10, fh.seek(asioext::seek_origin::from_current, 10));
  BOOST_REQUIRE_EQUAL(10, fh.position());
  BOOST_REQUIRE_EQUAL(20, fh.seek(asioext::seek_origin::from_current, 10));
  BOOST_REQUIRE_EQUAL(20, fh.position());
  BOOST_REQUIRE_EQUAL(10, fh.seek(asioext::seek_origin::from_current, -10));

  char buffer[10] = {0, };
  BOOST_REQUIRE_EQUAL(10, asio::write(fh, asio::buffer(buffer)));

  // size is now 20
  BOOST_REQUIRE_EQUAL(10, fh.seek(asioext::seek_origin::from_end, -10));
}

BOOST_AUTO_TEST_CASE(get_size)
{
  test_file_rm_guard rguard1(test_filename);

  asioext::unique_file_handle fh;

  asioext::error_code ec;
  fh = asioext::open(test_filename,
                     asioext::open_flags::access_write |
                     asioext::open_flags::create_always,
                     asioext::file_perms::create_default,
                     asioext::file_attrs::none, ec);
  BOOST_REQUIRE_MESSAGE(!ec, "ec: " << ec);

  BOOST_REQUIRE_EQUAL(0, fh.size());
  BOOST_REQUIRE_EQUAL(test_data_size,
                      asio::write(fh, asio::buffer(test_data,
                                                   test_data_size)));
  BOOST_REQUIRE_EQUAL(test_data_size, fh.size());
}

BOOST_AUTO_TEST_CASE(set_size)
{
  test_file_rm_guard rguard1(test_filename);

  asioext::unique_file_handle fh;

  asioext::error_code ec;
  fh = asioext::open(test_filename,
                     asioext::open_flags::access_write |
                     asioext::open_flags::create_always,
                     asioext::file_perms::create_default,
                     asioext::file_attrs::none, ec);
  BOOST_REQUIRE_MESSAGE(!ec, "ec: " << ec);

  BOOST_REQUIRE_EQUAL(0, fh.size());
  fh.size(128, ec);
  BOOST_REQUIRE_MESSAGE(!ec, "ec: " << ec);
  BOOST_REQUIRE_EQUAL(128, fh.size());
  BOOST_REQUIRE_EQUAL(0, fh.position());

  BOOST_REQUIRE_EQUAL(test_data_size,
                      asio::write(fh, asio::buffer(test_data,
                                                   test_data_size)));
  BOOST_REQUIRE_EQUAL(128, fh.size());
}

BOOST_AUTO_TEST_CASE(get_times)
{
  const std::time_t now = std::time(nullptr);

  asioext::error_code ec;
  asioext::unique_file_handle fh = asioext::open(test_filename,
    asioext::open_flags::access_write |
    asioext::open_flags::create_always,
    asioext::file_perms::create_default,
    asioext::file_attrs::none, ec);
  BOOST_REQUIRE_MESSAGE(!ec, "ec: " << ec);

  file_times times1 = fh.times(ec);
  BOOST_REQUIRE_MESSAGE(!ec, "ec: " << ec);

  file_times times2 = fh.times(ec);
  BOOST_REQUIRE_MESSAGE(!ec, "ec: " << ec);
  BOOST_REQUIRE(times1 == times2);

#if defined(ASIOEXT_WINDOWS)
  BOOST_CHECK_LE(now, file_clock::to_time_t(times1.ctime));
#endif
  BOOST_CHECK_LE(now, file_clock::to_time_t(times1.atime));
  BOOST_CHECK_LE(now, file_clock::to_time_t(times1.mtime));
}

BOOST_AUTO_TEST_CASE(set_times_auto)
{
  asioext::error_code ec;
  asioext::unique_file_handle fh = asioext::open(test_filename,
    asioext::open_flags::access_write |
    asioext::open_flags::create_always,
    asioext::file_perms::create_default,
    asioext::file_attrs::none, ec);
  BOOST_REQUIRE_MESSAGE(!ec, "ec: " << ec);

  const file_times times1 = fh.times(ec);
  BOOST_REQUIRE_MESSAGE(!ec, "ec: " << ec);

  char buffer[10] = {0, };
  BOOST_REQUIRE_EQUAL(10, asio::write(fh, asio::buffer(buffer)));

  fh.close();

  std::this_thread::sleep_for(std::chrono::seconds(3));

  fh = asioext::open(test_filename,
    asioext::open_flags::access_write |
    asioext::open_flags::open_existing,
    asioext::file_perms::create_default,
    asioext::file_attrs::none, ec);
  BOOST_REQUIRE_MESSAGE(!ec, "ec: " << ec);

  char buffer2[20] = {0, };
  BOOST_REQUIRE_EQUAL(20, asio::write(fh, asio::buffer(buffer2)));

  fh.close();

  fh = asioext::open(test_filename,
    asioext::open_flags::access_write |
    asioext::open_flags::open_existing,
    asioext::file_perms::create_default,
    asioext::file_attrs::none, ec);
  BOOST_REQUIRE_MESSAGE(!ec, "ec: " << ec);

  const file_times times2 = fh.times(ec);
  BOOST_REQUIRE_MESSAGE(!ec, "ec: " << ec);

  BOOST_CHECK_EQUAL(times1.ctime.time_since_epoch().count(),
                    times2.ctime.time_since_epoch().count());
  BOOST_CHECK_LE(times1.atime.time_since_epoch().count(),
                 times2.atime.time_since_epoch().count());
  BOOST_CHECK_LE(times1.mtime.time_since_epoch().count(),
                 times2.mtime.time_since_epoch().count());
}

BOOST_AUTO_TEST_CASE(set_times_manual)
{
  static const std::time_t test_time = 1405706349;

  asioext::error_code ec;
  asioext::unique_file_handle fh = asioext::open(test_filename,
    asioext::open_flags::access_write |
    asioext::open_flags::create_always,
    asioext::file_perms::create_default,
    asioext::file_attrs::none, ec);
  BOOST_REQUIRE_MESSAGE(!ec, "ec: " << ec);

  file_times times1 = fh.times(ec);
  BOOST_REQUIRE_MESSAGE(!ec, "ec: " << ec);

  file_times times2;
  times2.mtime = file_clock::from_time_t(test_time);
  fh.times(times2, ec);
  BOOST_REQUIRE_MESSAGE(!ec, "ec: " << ec);

  file_times times3 = fh.times(ec);
  BOOST_REQUIRE_MESSAGE(!ec, "ec: " << ec);

  BOOST_CHECK_EQUAL(times2.mtime.time_since_epoch().count(),
                    times3.mtime.time_since_epoch().count());
#if defined(ASIOEXT_WINDOWS)
  BOOST_CHECK_EQUAL(times1.ctime.time_since_epoch().count(),
                    times3.ctime.time_since_epoch().count());
#endif
}

BOOST_AUTO_TEST_SUITE_END()

ASIOEXT_NS_END
