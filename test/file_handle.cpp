#include "asioext/file_handle.hpp"
#include "asioext/open_flags.hpp"

#if defined(ASIOEXT_USE_BOOST_ASIO)
# include <boost/asio/write.hpp>
# include <boost/asio/read.hpp>
#else
# include <asio/write.hpp>
# include <asio/read.hpp>
#endif

#include <boost/test/unit_test.hpp>

#if defined(ASIOEXT_USE_BOOST_ASIO)
namespace asio = asioext::asio;
#endif

BOOST_AUTO_TEST_SUITE(asioext_file_handle)

BOOST_AUTO_TEST_CASE(empty)
{
  asioext::file_handle fh;
  BOOST_CHECK(!fh.is_open());

  BOOST_CHECK_NO_THROW(fh.close());
}

BOOST_AUTO_TEST_CASE(open_fail)
{
  using namespace asioext::open_flags;
  asioext::file_handle fh;

  asioext::error_code ec;
  fh.open("nosuchfile", access_read | open_existing, ec);

  BOOST_REQUIRE(!!ec);
  BOOST_CHECK(!fh.is_open());

  BOOST_CHECK_NO_THROW(fh.close());
}

BOOST_AUTO_TEST_CASE(open_succeed)
{
  using namespace asioext::open_flags;
  asioext::file_handle fh;

  asioext::error_code ec;
  fh.open("test_file_1", access_write | open_always, ec);

  BOOST_REQUIRE(!ec);
  BOOST_CHECK(fh.is_open());

  BOOST_CHECK_NO_THROW(fh.close());
  BOOST_CHECK(!fh.is_open());
}

BOOST_AUTO_TEST_CASE(assign)
{
  using namespace asioext::open_flags;
  asioext::file_handle fh, fh2;

  asioext::error_code ec;
  fh.open("test_file_1", access_write | create_always, ec);
  BOOST_REQUIRE(!ec);

  const asioext::file_handle::native_handle_type h = fh.native_handle();
  fh2.assign(h, ec);
  fh.leak();

  BOOST_REQUIRE(!ec);
  BOOST_CHECK(!fh.is_open());
  BOOST_CHECK(fh2.is_open());
  BOOST_REQUIRE_EQUAL(fh2.native_handle(), h);
}

BOOST_AUTO_TEST_CASE(read_write)
{
  using namespace asioext::open_flags;
  asioext::file_handle fh;

  asioext::error_code ec;
  fh.open("test_file_1", access_write | create_always, ec);
  BOOST_REQUIRE(!ec);

  const char data[] = "hello world!";
  const std::size_t data_size = sizeof(data) - 1;

  BOOST_REQUIRE_EQUAL(0, asio::write(fh, asio::buffer(data, 0)));
  BOOST_REQUIRE_EQUAL(data_size, asio::write(fh, asio::buffer(data, data_size)));

  BOOST_REQUIRE_NO_THROW(fh.close());

  fh.open("test_file_1", access_read | open_existing, ec);
  BOOST_REQUIRE(!ec);

  char buffer[128];

  BOOST_REQUIRE_EQUAL(data_size, asio::read(fh, asio::buffer(buffer, data_size)));
  BOOST_REQUIRE_EQUAL(0, std::memcmp(data, buffer, data_size));
}

BOOST_AUTO_TEST_CASE(position_and_size)
{
  using namespace asioext::open_flags;
  asioext::file_handle fh;

  asioext::error_code ec;
  fh.open("test_file_1", access_write | create_always, ec);
  BOOST_REQUIRE(!ec);

  const char data[] = "hello world!";
  const std::size_t data_size = sizeof(data) - 1;

  BOOST_REQUIRE_EQUAL(0, fh.position());
  BOOST_REQUIRE_EQUAL(data_size, asio::write(fh, asio::buffer(data, data_size)));
  BOOST_REQUIRE_EQUAL(data_size, fh.position());
  BOOST_REQUIRE_EQUAL(data_size, fh.size());
}

BOOST_AUTO_TEST_CASE(seek)
{
  using namespace asioext::open_flags;
  asioext::file_handle fh;

  asioext::error_code ec;
  fh.open("test_file_1", access_write | create_always, ec);
  BOOST_REQUIRE(!ec);

  BOOST_REQUIRE_EQUAL(0, fh.seek(asioext::file_handle::from_current, 0));
  BOOST_REQUIRE_EQUAL(10, fh.seek(asioext::file_handle::from_current, 10));
  BOOST_REQUIRE_EQUAL(20, fh.seek(asioext::file_handle::from_current, 10));
  BOOST_REQUIRE_EQUAL(10, fh.seek(asioext::file_handle::from_current, -10));

  char buffer[10] = {0, };
  BOOST_REQUIRE_EQUAL(10, asio::write(fh, asio::buffer(buffer)));

  // size is now 20
  BOOST_REQUIRE_EQUAL(10, fh.seek(asioext::file_handle::from_end, -10));
}

BOOST_AUTO_TEST_SUITE_END()
