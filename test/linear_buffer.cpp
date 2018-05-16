#include "asioext/linear_buffer.hpp"

#include <boost/test/unit_test.hpp>

ASIOEXT_NS_BEGIN

BOOST_AUTO_TEST_SUITE(asioext_linear_buffer)

typedef dynamic_linear_buffer<std::allocator<uint8_t>> dynbuf_type;

BOOST_AUTO_TEST_CASE(empty)
{
  linear_buffer x1;
  BOOST_CHECK_EQUAL(0, x1.size());
  BOOST_CHECK_EQUAL(0, x1.capacity());
}

BOOST_AUTO_TEST_CASE(prepare_commit)
{
  linear_buffer b;
  dynbuf_type x1(b);
  dynbuf_type::mutable_buffers_type b1 = x1.prepare(2);
  asio::buffer_cast<char*>(b1)[0] = 'A';
  asio::buffer_cast<char*>(b1)[1] = 'B';
  x1.commit(2);

  BOOST_REQUIRE_EQUAL(2, x1.size());
  BOOST_REQUIRE_GE(2, x1.capacity());
  BOOST_REQUIRE_EQUAL('A', asio::buffer_cast<const char*>(x1.data())[0]);
  BOOST_REQUIRE_EQUAL('B', asio::buffer_cast<const char*>(x1.data())[1]);

  dynbuf_type::mutable_buffers_type b2 = x1.prepare(2);
  asio::buffer_cast<char*>(b2)[0] = 'C';
  asio::buffer_cast<char*>(b2)[1] = 'D';
  x1.commit(2);

  BOOST_REQUIRE_EQUAL(4, x1.size());
  BOOST_REQUIRE_LE(4, x1.capacity());
  BOOST_REQUIRE_EQUAL('A', asio::buffer_cast<const char*>(x1.data())[0]);
  BOOST_REQUIRE_EQUAL('B', asio::buffer_cast<const char*>(x1.data())[1]);
  BOOST_REQUIRE_EQUAL('C', asio::buffer_cast<const char*>(x1.data())[2]);
  BOOST_REQUIRE_EQUAL('D', asio::buffer_cast<const char*>(x1.data())[3]);

  dynbuf_type::mutable_buffers_type b3 = x1.prepare(1);
  asio::buffer_cast<char*>(b3)[0] = 'E';
  dynbuf_type::mutable_buffers_type b4 = x1.prepare(1);
  asio::buffer_cast<char*>(b4)[0] = 'F';
  x1.commit(1);

  BOOST_REQUIRE_EQUAL(5, x1.size());
  BOOST_REQUIRE_LE(5, x1.capacity());
  BOOST_REQUIRE_EQUAL('A', asio::buffer_cast<const char*>(x1.data())[0]);
  BOOST_REQUIRE_EQUAL('B', asio::buffer_cast<const char*>(x1.data())[1]);
  BOOST_REQUIRE_EQUAL('C', asio::buffer_cast<const char*>(x1.data())[2]);
  BOOST_REQUIRE_EQUAL('D', asio::buffer_cast<const char*>(x1.data())[3]);
  BOOST_REQUIRE_EQUAL('F', asio::buffer_cast<const char*>(x1.data())[4]);
}

BOOST_AUTO_TEST_CASE(consume)
{
  linear_buffer b;
  dynbuf_type x1(b);
  dynbuf_type::mutable_buffers_type b1 = x1.prepare(4);
  asio::buffer_cast<char*>(b1)[0] = 'A';
  asio::buffer_cast<char*>(b1)[1] = 'B';
  asio::buffer_cast<char*>(b1)[2] = 'C';
  asio::buffer_cast<char*>(b1)[3] = 'D';
  x1.commit(4);
  x1.consume(2);

  BOOST_REQUIRE_EQUAL(2, x1.size());
  BOOST_REQUIRE_LE(2, x1.capacity());
  BOOST_REQUIRE_EQUAL('C', asio::buffer_cast<const char*>(x1.data())[0]);
  BOOST_REQUIRE_EQUAL('D', asio::buffer_cast<const char*>(x1.data())[1]);

  x1.consume(2);
  BOOST_REQUIRE_EQUAL(0, x1.size());
  BOOST_REQUIRE_LE(0, x1.capacity());
}

BOOST_AUTO_TEST_CASE(max_size)
{
  linear_buffer b;
  dynamic_linear_buffer<std::allocator<uint8_t>> x1(b, 4);
  BOOST_CHECK_EQUAL(4, x1.max_size());

  BOOST_CHECK_NO_THROW(x1.prepare(4));
  BOOST_CHECK_THROW(x1.prepare(5), std::length_error);

  error_code ec;
  x1.prepare(4, ec);
  BOOST_CHECK(!ec);
  x1.prepare(5, ec);
  BOOST_CHECK_EQUAL(asio::error::no_memory, ec);

  x1.prepare(2);
  x1.commit(2);

  BOOST_CHECK_NO_THROW(x1.prepare(2));
  BOOST_CHECK_THROW(x1.prepare(3), std::length_error);

  x1.prepare(2, ec);
  BOOST_CHECK(!ec);
  x1.prepare(3, ec);
  BOOST_CHECK_EQUAL(asio::error::no_memory, ec);
}

BOOST_AUTO_TEST_CASE(move)
{
}

BOOST_AUTO_TEST_SUITE_END()

ASIOEXT_NS_END
