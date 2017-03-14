#include "test_file_rm_guard.hpp"

#include "asioext/scoped_file_handle.hpp"
#include "asioext/open_flags.hpp"

#if defined(ASIOEXT_USE_BOOST_ASIO)
# include <boost/asio/write.hpp>
# include <boost/asio/read.hpp>
#else
# include <asio/write.hpp>
# include <asio/read.hpp>
#endif

#include <boost/test/unit_test.hpp>

ASIOEXT_NS_BEGIN

BOOST_AUTO_TEST_SUITE(asioext_scoped_file_handle)

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
  asioext::scoped_file_handle fh;
  BOOST_CHECK(!fh.is_open());
  BOOST_CHECK_NO_THROW(fh.close());
}

BOOST_AUTO_TEST_CASE(constructor)
{
  test_file_rm_guard rguard1(empty_filename);

  asioext::error_code ec;
  asioext::scoped_file_handle f1("nosuchfile",
                                 open_flags::access_read |
                                 open_flags::open_existing,
                                 asioext::file_perms::create_default,
                                 asioext::file_attrs::none, ec);
  BOOST_REQUIRE(ec);
  asioext::scoped_file_handle f2(empty_filename,
                                 open_flags::access_write |
                                 open_flags::open_always,
                                 asioext::file_perms::create_default,
                                 asioext::file_attrs::none, ec);
  BOOST_REQUIRE(!ec);
#if defined(ASIOEXT_WINDOWS)
  test_file_rm_guard rguard2(empty_filenamew);
  asioext::scoped_file_handle f3(L"nosuchfile",
                                 open_flags::access_read |
                                 open_flags::open_existing,
                                 asioext::file_perms::create_default,
                                 asioext::file_attrs::none, ec);
  BOOST_REQUIRE(ec);
  asioext::scoped_file_handle f4(empty_filenamew,
                                 open_flags::access_write |
                                 open_flags::open_always,
                                 asioext::file_perms::create_default,
                                 asioext::file_attrs::none, ec);
  BOOST_REQUIRE(!ec);
#endif
}

BOOST_AUTO_TEST_CASE(open)
{
  test_file_rm_guard rguard1(empty_filename);
#if defined(ASIOEXT_WINDOWS)
  test_file_rm_guard rguard2(empty_filenamew);
#endif

  asioext::scoped_file_handle fh;

  asioext::error_code ec;
  fh.open("nosuchfile",
          open_flags::access_read | open_flags::open_existing,
          asioext::file_perms::create_default,
          asioext::file_attrs::none, ec);

  BOOST_REQUIRE(!!ec);
  BOOST_CHECK(!fh.is_open());
  BOOST_CHECK_NO_THROW(fh.close());

#if defined(ASIOEXT_WINDOWS)
  fh.open(L"nosuchfile",
          open_flags::access_read | open_flags::open_existing,
          asioext::file_perms::create_default,
          asioext::file_attrs::none, ec);

  BOOST_REQUIRE(!!ec);
  BOOST_CHECK(!fh.is_open());
  BOOST_CHECK_NO_THROW(fh.close());
#endif

  fh.open(empty_filename,
          open_flags::access_write | open_flags::open_always,
          asioext::file_perms::create_default,
          asioext::file_attrs::none, ec);

  BOOST_REQUIRE(!ec);
  BOOST_CHECK(fh.is_open());

  BOOST_CHECK_NO_THROW(fh.close());
  BOOST_CHECK(!fh.is_open());

#if defined(ASIOEXT_WINDOWS)
  fh.open(empty_filenamew,
          open_flags::access_write | open_flags::open_always,
          asioext::file_perms::create_default,
          asioext::file_attrs::none, ec);
  BOOST_REQUIRE(!ec);
  BOOST_CHECK(fh.is_open());
#endif
}

BOOST_AUTO_TEST_CASE(leak_reset)
{
  test_file_rm_guard rguard1(empty_filename);

  asioext::scoped_file_handle fh, fh2;

  asioext::error_code ec;
  fh.open(empty_filename,
          open_flags::access_write | open_flags::create_always,
          asioext::file_perms::create_default,
          asioext::file_attrs::none, ec);
  BOOST_REQUIRE(!ec);

  asioext::file_handle h = fh.get();
  fh2.reset(h, ec);
  fh.leak();

  BOOST_REQUIRE(!ec);
  BOOST_CHECK(!fh.is_open());
  BOOST_CHECK(fh2.is_open());
  BOOST_REQUIRE_EQUAL(fh2.get().native_handle(), h.native_handle());
}

BOOST_AUTO_TEST_CASE(read_write)
{
  test_file_rm_guard rguard1(test_filename);

  asioext::scoped_file_handle fh;

  asioext::error_code ec;
  fh.open(test_filename,
          open_flags::access_write | open_flags::create_always,
          asioext::file_perms::create_default,
          asioext::file_attrs::none, ec);
  BOOST_REQUIRE(!ec);

  BOOST_REQUIRE_EQUAL(0, asio::write(fh, asio::buffer(test_data, 0)));
  BOOST_REQUIRE_EQUAL(test_data_size,
                      asio::write(fh, asio::buffer(test_data,
                                                   test_data_size)));

  asio::const_buffers_1 empty_buffer1(nullptr, 0);
  BOOST_REQUIRE_EQUAL(0, fh.write_some_at(0, empty_buffer1));
  BOOST_REQUIRE_EQUAL(0, fh.write_some_at(1, empty_buffer1));

  BOOST_REQUIRE_NO_THROW(fh.close());

  fh.open(test_filename,
          open_flags::access_read | open_flags::open_existing,
          asioext::file_perms::create_default,
          asioext::file_attrs::none, ec);
  BOOST_REQUIRE(!ec);

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

BOOST_AUTO_TEST_CASE(position_and_size)
{
  test_file_rm_guard rguard1(test_filename);

  asioext::scoped_file_handle fh;

  asioext::error_code ec;
  fh.open(test_filename,
          open_flags::access_write | open_flags::create_always,
          asioext::file_perms::create_default,
          asioext::file_attrs::none, ec);
  BOOST_REQUIRE(!ec);

  BOOST_REQUIRE_EQUAL(0, fh.position());
  BOOST_REQUIRE_EQUAL(test_data_size,
                      asio::write(fh, asio::buffer(test_data,
                                                   test_data_size)));
  BOOST_REQUIRE_EQUAL(test_data_size, fh.position());
  BOOST_REQUIRE_EQUAL(test_data_size, fh.size());
}

BOOST_AUTO_TEST_CASE(seek)
{
  test_file_rm_guard rguard1(test_filename);

  asioext::scoped_file_handle fh;

  asioext::error_code ec;
  fh.open(test_filename,
          open_flags::access_write | open_flags::create_always,
          asioext::file_perms::create_default,
          asioext::file_attrs::none, ec);
  BOOST_REQUIRE(!ec);

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

BOOST_AUTO_TEST_SUITE_END()

ASIOEXT_NS_END
