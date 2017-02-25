#include "test_file_rm_guard.hpp"

#include "asioext/read_file.hpp"
#include "asioext/write_file.hpp"

#include <boost/test/unit_test.hpp>
#include <boost/mpl/list.hpp>
#include <boost/array.hpp>

ASIOEXT_NS_BEGIN

BOOST_AUTO_TEST_SUITE(asioext_write_file)

// BOOST_AUTO_TEST_SUITE() gives us a unique NS, so we don't need to
// prefix our variables.

static const char* test_filename = "asioext_writefile_test";
static const wchar_t* test_filenamew = L"asioext_writefile_test";

static const char test_data[] = "hello world!";
static const std::size_t test_data_size = sizeof(test_data) - 1;

template <class ConstBufferSequence>
static boost::test_tools::predicate_result compare_written(
    const char* filename, const ConstBufferSequence& c)
{
  std::vector<unsigned char> data;
  asioext::read_file(filename, data);

  const std::size_t c_size = asio::buffer_size(c);
  if (data.size() != c_size) {
    boost::test_tools::predicate_result res(false);
    res.message() << "Different sizes [" << data.size() << "!=" << c_size << "]";
    return res;
  }

  typename ConstBufferSequence::const_iterator it = c.begin();
  for (std::size_t i = 0, j = 0; i != c_size; ++i, ++j) {
    while (j == asio::buffer_size(*it)) {
      ++it;
      j = 0;
    }

    if (data[i] == asio::buffer_cast<const unsigned char*>(*it)[j])
      continue;

    boost::test_tools::predicate_result res(false);
    res.message() << "Mismatch at " << i;
    res.message() << " [" << test_data[i] << " != ";
    res.message() << asio::buffer_cast<const unsigned char*>(*it)[j] << "]";
    return res;
  }

  return true;
}

BOOST_AUTO_TEST_CASE(empty)
{
  test_file_rm_guard rguard1(test_filename);

  asioext::write_file(test_filename, asio::buffer(asio::const_buffer()));
  BOOST_CHECK(compare_written(test_filename,
                              asio::buffer(asio::const_buffer())));
}

BOOST_AUTO_TEST_CASE(one_buffer)
{
  test_file_rm_guard rguard1(test_filename);

  const asio::const_buffer buffer(test_data, test_data_size);
  asioext::write_file(test_filename, asio::buffer(buffer));
  BOOST_CHECK(compare_written(test_filename, asio::buffer(buffer)));
}

BOOST_AUTO_TEST_CASE(two_buffers)
{
  test_file_rm_guard rguard1(test_filename);

  const boost::array<asio::const_buffer, 2> buffers = {
    asio::const_buffer(test_data, test_data_size),
    asio::const_buffer(test_data, test_data_size),
  };

  asioext::write_file(test_filename, buffers);
  BOOST_CHECK(compare_written(test_filename, buffers));
}

BOOST_AUTO_TEST_SUITE_END()

ASIOEXT_NS_END
