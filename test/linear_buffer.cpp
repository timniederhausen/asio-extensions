#include "asioext/linear_buffer.hpp"

#include "asioext/detail/asio_version.hpp"

#include <boost/test/unit_test.hpp>

ASIOEXT_NS_BEGIN

#if (ASIOEXT_ASIO_VERSION >= 101400)
# if !defined(ASIOEXT_NO_DYNAMIC_BUFFER_V1)
static_assert(asio::is_dynamic_buffer_v1<asioext::dynamic_linear_buffer<
                  std::allocator<char>>>::value,
              "concept check");
# endif
static_assert(asio::is_dynamic_buffer_v2<asioext::dynamic_linear_buffer<
                  std::allocator<char>>>::value,
              "concept check");
#else
static_assert(asio::is_dynamic_buffer<asioext::dynamic_linear_buffer<
                  std::allocator<char>>>::value,
              "concept check");
#endif

BOOST_AUTO_TEST_SUITE(asioext_linear_buffer)

BOOST_AUTO_TEST_CASE(basic_construction)
{
  linear_buffer x1;
  BOOST_CHECK_EQUAL(0, x1.size());
  BOOST_CHECK_EQUAL(0, x1.capacity());

  linear_buffer x2(16, 64);
  BOOST_CHECK_EQUAL(16, x2.size());
  BOOST_CHECK_LE(5, x2.capacity());
  BOOST_CHECK_EQUAL(64, x2.max_size());
}

BOOST_AUTO_TEST_CASE(copy)
{
  linear_buffer a;
  a.resize(5);
  std::memcpy(a.data(), "HELLO", 5);
  BOOST_REQUIRE_EQUAL(5, a.size());
  BOOST_REQUIRE_LE(5, a.capacity());

  linear_buffer b(a);
  BOOST_REQUIRE_EQUAL(5, b.size());
  BOOST_REQUIRE_LE(5, b.capacity());
  BOOST_REQUIRE_EQUAL(std::string(reinterpret_cast<const char*>(b.data()), 5),
                      "HELLO");
}

#if defined(ASIOEXT_HAS_MOVE)
BOOST_AUTO_TEST_CASE(move)
{
  linear_buffer a;
  a.resize(5);
  std::memcpy(a.data(), "HELLO", 5);
  BOOST_REQUIRE_EQUAL(5, a.size());
  BOOST_REQUIRE_LE(5, a.capacity());

  linear_buffer b(std::move(a));
  BOOST_REQUIRE_EQUAL(0, a.size());
  BOOST_REQUIRE_EQUAL(0, a.capacity());
  BOOST_REQUIRE_EQUAL(5, b.size());
  BOOST_REQUIRE_LE(5, b.capacity());
  BOOST_REQUIRE_EQUAL(std::string(reinterpret_cast<const char*>(b.data()), 5),
                      "HELLO");
}
#endif

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(asioext_dynamic_linear_buffer)

typedef dynamic_linear_buffer<std::allocator<uint8_t>> dynbuf_type;

BOOST_AUTO_TEST_CASE(prepare_commit)
{
  linear_buffer a;
  dynbuf_type x1(a);
  dynbuf_type::mutable_buffers_type b1 = x1.prepare(2);
  asio::buffer_cast<char*>(b1)[0] = 'A';
  asio::buffer_cast<char*>(b1)[1] = 'B';
  x1.commit(2);

  BOOST_REQUIRE_EQUAL(2, x1.size());
  BOOST_REQUIRE_LE(2, x1.capacity());
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

BOOST_AUTO_TEST_CASE(grow_shrink)
{
  linear_buffer a;
  dynbuf_type x1(a);
  BOOST_CHECK_EQUAL(0, x1.size());

  x1.grow(4);
  BOOST_CHECK_EQUAL(4, x1.size());
  x1.grow(4);
  BOOST_CHECK_EQUAL(8, x1.size());

  x1.shrink(4);
  BOOST_CHECK_EQUAL(4, x1.size());
  x1.shrink(4);
  BOOST_CHECK_EQUAL(0, x1.size());
}

BOOST_AUTO_TEST_CASE(consume)
{
  linear_buffer a;
  dynbuf_type x1(a);
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
  linear_buffer a;
  dynbuf_type x1(a, 4);
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

#if defined(ASIOEXT_HAS_MOVE)
BOOST_AUTO_TEST_CASE(move)
{
  linear_buffer a(16, 64);
  dynbuf_type x1(a);
  dynbuf_type x2(std::move(x1));
  BOOST_CHECK_EQUAL(16, x2.size());
  BOOST_CHECK_EQUAL(64, x2.max_size());
}
#endif

BOOST_AUTO_TEST_SUITE_END()

ASIOEXT_NS_END
