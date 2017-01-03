#include "asioext/scoped_file_handle.hpp"
#include "asioext/open_flags.hpp"
#include "asioext/read_file.hpp"

#if defined(ASIOEXT_USE_BOOST_ASIO)
# include <boost/asio/write.hpp>
#else
# include <asio/write.hpp>
#endif

#include <boost/test/unit_test.hpp>

#if defined(ASIOEXT_USE_BOOST_ASIO)
namespace asio = asioext::asio;
#endif

BOOST_AUTO_TEST_SUITE(asioext_read_file)

void write_test_file()
{
  asioext::scoped_file_handle fh("test_file_2",
                                 asioext::open_flags::access_write |
                                 asioext::open_flags::create_always);

  const char data[] = "hello world!";
  const std::size_t data_size = sizeof(data) - 1;
  asio::write(fh, asio::buffer(data, data_size));
}

BOOST_AUTO_TEST_CASE(empty)
{
  asioext::scoped_file_handle fh("test_file_1",
                                 asioext::open_flags::access_write |
                                 asioext::open_flags::create_always);
  fh.close();

  std::string str;
  asioext::error_code ec;
  asioext::read_file("test_file_1", str, ec);

  BOOST_REQUIRE(!ec);
  BOOST_CHECK_EQUAL(0, str.size());

  std::vector<char> vec;
  asioext::read_file("test_file_1", vec, ec);

  BOOST_REQUIRE(!ec);
  BOOST_CHECK_EQUAL(0, vec.size());
}

BOOST_AUTO_TEST_CASE(read_file_char)
{
  write_test_file();

  std::string str;
  asioext::error_code ec;
  asioext::read_file("test_file_2", str, ec);

  BOOST_REQUIRE(!ec);
  BOOST_CHECK_EQUAL(12, str.size());

  asioext::read_file("test_file_2", str, ec);

  BOOST_REQUIRE(!ec);
  BOOST_CHECK_EQUAL(12, str.size());

  std::vector<char> vec;
  asioext::read_file("test_file_2", vec, ec);

  BOOST_REQUIRE(!ec);
  BOOST_CHECK_EQUAL(12, vec.size());
}

#if defined(ASIOEXT_WINDOWS)
BOOST_AUTO_TEST_CASE(read_file_wchar)
{
  write_test_file();

  std::string str;
  asioext::error_code ec;
  asioext::read_file(L"test_file_2", str, ec);

  BOOST_REQUIRE(!ec);
  BOOST_CHECK_EQUAL(12, str.size());

  asioext::read_file(L"test_file_2", str, ec);

  BOOST_REQUIRE(!ec);
  BOOST_CHECK_EQUAL(12, str.size());

  std::vector<char> vec;
  asioext::read_file(L"test_file_2", vec, ec);

  BOOST_REQUIRE(!ec);
  BOOST_CHECK_EQUAL(12, vec.size());
}
#endif

BOOST_AUTO_TEST_SUITE_END()
