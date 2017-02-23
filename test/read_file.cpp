#include "test_file_writer.hpp"

#include "asioext/read_file.hpp"
#include "asioext/open.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/mpl/list.hpp>

ASIOEXT_NS_BEGIN

BOOST_AUTO_TEST_SUITE(asioext_read_file)

// BOOST_AUTO_TEST_SUITE() gives us a unique NS, so we don't need to
// prefix our variables.

static const char* empty_filename = "asioext_readfile_empty";

static const char* test_filename = "asioext_readfile_test";
static const wchar_t* test_filenamew = L"asioext_readfile_test";
static const char test_data[] = "hello world!";
static const std::size_t test_data_size = sizeof(test_data) - 1;

typedef boost::mpl::list<char, signed char, unsigned char> char_types;

static void write_empty_file()
{
  static test_file_writer file(empty_filename, 0, 0);
}

static void write_test_file()
{
  static test_file_writer file(test_filename, test_data, test_data_size);
}

template <typename T1, typename T2>
static bool compare_characters(T1 a1, T2 a2)
{
  return static_cast<unsigned char>(a1) == static_cast<unsigned char>(a2);
}

template <class C>
static boost::test_tools::predicate_result compare_with_test_data(const C& c)
{
  if (test_data_size != c.size()) {
    boost::test_tools::predicate_result res(false);
    res.message() << "Different sizes [" << test_data_size << "!=" << c.size() << "]";
    return res;
  }

  for (std::size_t i = 0; i != test_data_size; ++i) {
    if (compare_characters(test_data[i], c[i]))
      continue;

    boost::test_tools::predicate_result res(false);
    res.message() << "Mismatch at " << i;
    res.message() << " [" << test_data[i] << " != " << c[i] << "]";
    return res;
  }

  return true;
}

BOOST_AUTO_TEST_CASE(empty)
{
  write_empty_file();

  std::string str;
  asioext::error_code ec;
  asioext::read_file(empty_filename, str, ec);

  BOOST_REQUIRE(!ec);
  BOOST_CHECK_EQUAL(0, str.size());

  std::vector<char> vec;
  asioext::read_file(empty_filename, vec, ec);

  BOOST_REQUIRE(!ec);
  BOOST_CHECK_EQUAL(0, vec.size());

  // MutableBufferSequence

  char buffer[1];
  asioext::read_file(empty_filename, asio::buffer(buffer, 0), ec);
  BOOST_REQUIRE(!ec);

  asioext::read_file(empty_filename, asio::buffer(buffer, 1), ec);
  BOOST_CHECK_EQUAL(ec, asio::error::eof);
}

BOOST_AUTO_TEST_CASE(nonexistent)
{
  std::string str;
  asioext::error_code ec;

  asioext::read_file("nosuchfile", str, ec);

  BOOST_REQUIRE(ec);
  BOOST_CHECK_EQUAL(0, str.size());

  // MutableBufferSequence

  char buffer[1];
  asioext::read_file("nosuchfile", asio::buffer(buffer, 0), ec);
  BOOST_REQUIRE(ec);
}

BOOST_AUTO_TEST_CASE(read_file_string)
{
  write_test_file();

  std::string str;
  asioext::error_code ec;

  // read a file into an empty string.
  asioext::read_file(test_filename, str, ec);

  BOOST_REQUIRE(!ec);
  BOOST_CHECK_EQUAL(test_data_size, str.size());
  BOOST_CHECK_EQUAL(test_data, str);

  // re-use the already filled string object to read a file.
  asioext::read_file(test_filename, str, ec);

  BOOST_REQUIRE(!ec);
  BOOST_CHECK_EQUAL(test_data_size, str.size());
  BOOST_CHECK_EQUAL(test_data, str);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(read_file_vector, T, char_types)
{
  write_test_file();

  asioext::error_code ec;
  std::vector<T> vec;

  // read a file into an empty vector.
  asioext::read_file(test_filename, vec, ec);

  BOOST_REQUIRE(!ec);
  BOOST_CHECK(compare_with_test_data(vec));

  // re-use the already filled vector object to read a file.
  asioext::read_file(test_filename, vec, ec);

  BOOST_REQUIRE(!ec);
  BOOST_CHECK(compare_with_test_data(vec));
}

BOOST_AUTO_TEST_CASE(read_file_buffer)
{
  write_test_file();

  char buffer[test_data_size + 1] = {'\0'};
  asioext::error_code ec;

  asioext::read_file(test_filename,
                     asio::buffer(buffer, test_data_size),
                     ec);

  BOOST_REQUIRE(!ec);
  BOOST_CHECK_EQUAL(test_data, buffer);
}

#if defined(ASIOEXT_WINDOWS)
BOOST_AUTO_TEST_CASE(read_file_wide_filename)
{
  write_test_file();

  std::string str;
  asioext::error_code ec;

  // read a file into an empty string.
  asioext::read_file(test_filenamew, str, ec);

  BOOST_REQUIRE(!ec);
  BOOST_CHECK_EQUAL(test_data_size, str.size());
  BOOST_CHECK_EQUAL(test_data, str);

  // re-use the already filled string object to read a file.
  asioext::read_file(test_filenamew, str, ec);

  BOOST_REQUIRE(!ec);
  BOOST_CHECK_EQUAL(test_data_size, str.size());
  BOOST_CHECK_EQUAL(test_data, str);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(read_file_vector_wide_filename, T, char_types)
{
  write_test_file();

  asioext::error_code ec;
  std::vector<T> vec;

  // read a file into an empty vector.
  asioext::read_file(test_filenamew, vec, ec);

  BOOST_REQUIRE(!ec);
  BOOST_CHECK(compare_with_test_data(vec));

  // re-use the already filled vector object to read a file.
  asioext::read_file(test_filenamew, vec, ec);

  BOOST_REQUIRE(!ec);
  BOOST_CHECK(compare_with_test_data(vec));
}

BOOST_AUTO_TEST_CASE(read_file_buffer_wide_filename)
{
  write_test_file();

  char buffer[test_data_size + 1] = {'\0'};
  asioext::error_code ec;

  asioext::read_file(test_filenamew,
                     asio::buffer(buffer, test_data_size),
                     ec);

  BOOST_REQUIRE(!ec);
  BOOST_CHECK_EQUAL(test_data, buffer);
}
#endif

BOOST_AUTO_TEST_SUITE_END()

ASIOEXT_NS_END
