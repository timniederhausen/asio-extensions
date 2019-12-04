#include "asioext/detail/url_parser.hpp"

#include "asioext/error.hpp"

#include <boost/test/unit_test.hpp>

ASIOEXT_NS_BEGIN

BOOST_AUTO_TEST_SUITE(asioext_url_parser)

using asioext::detail::parsed_url;
using asioext::detail::parse_url;

parsed_url parse_url(std::string_view input, asioext::error_code& ec)
{
  parsed_url p;
  parse_url(input.data(), input.data() + input.size(), p, ec);
  return p;
}

BOOST_AUTO_TEST_CASE(simple)
{
  parsed_url p;
  asioext::error_code ec;

  p = parse_url("http://example.com/home/tim/test.txt", ec);
  BOOST_REQUIRE_MESSAGE(!ec, "ec:" << ec);
  BOOST_CHECK_EQUAL(p.scheme, "http");
  BOOST_CHECK_EQUAL(p.userinfo, "");
  BOOST_CHECK_EQUAL(p.host, "example.com");
  BOOST_CHECK_EQUAL(p.port, "");
  BOOST_CHECK_EQUAL(p.port_value, 0);
  BOOST_CHECK_EQUAL(p.path, "/home/tim/test.txt");
  BOOST_CHECK_EQUAL(p.query, "");
  BOOST_CHECK_EQUAL(p.fragment, "");

  p = parse_url("http://example.com:80/home/tim/test.txt", ec);
  BOOST_REQUIRE_MESSAGE(!ec, "ec:" << ec);
  BOOST_CHECK_EQUAL(p.scheme, "http");
  BOOST_CHECK_EQUAL(p.userinfo, "");
  BOOST_CHECK_EQUAL(p.host, "example.com");
  BOOST_CHECK_EQUAL(p.port, "80");
  BOOST_CHECK_EQUAL(p.port_value, 80);
  BOOST_CHECK_EQUAL(p.path, "/home/tim/test.txt");
  BOOST_CHECK_EQUAL(p.query, "");
  BOOST_CHECK_EQUAL(p.fragment, "");
}

BOOST_AUTO_TEST_CASE(simple_invalid)
{
  parsed_url p;
  asioext::error_code ec;

  (void)parse_url("h__p://example.com/home/tim/test.txt", ec);
  BOOST_CHECK(ec == url_error::invalid_character);

  (void)parse_url("http://example.com/ho me/tim/test.txt", ec);
  BOOST_CHECK(ec == url_error::invalid_character);
}

BOOST_AUTO_TEST_CASE(simple_with_userinfo)
{
  parsed_url p;
  asioext::error_code ec;

  p = parse_url("http://@example.com/home/tim/test.txt", ec);
  BOOST_REQUIRE_MESSAGE(!ec, "ec:" << ec);
  BOOST_CHECK_EQUAL(p.scheme, "http");
  BOOST_CHECK_EQUAL(p.userinfo, "");
  BOOST_CHECK_EQUAL(p.host, "example.com");
  BOOST_CHECK_EQUAL(p.port, "");
  BOOST_CHECK_EQUAL(p.port_value, 0);
  BOOST_CHECK_EQUAL(p.path, "/home/tim/test.txt");
  BOOST_CHECK_EQUAL(p.query, "");
  BOOST_CHECK_EQUAL(p.fragment, "");

  p = parse_url("http://test:test!$&'()*+,;=1@example.com:80/home/tim/test.txt", ec);
  BOOST_REQUIRE_MESSAGE(!ec, "ec:" << ec);
  BOOST_CHECK_EQUAL(p.scheme, "http");
  BOOST_CHECK_EQUAL(p.userinfo, "test:test!$&'()*+,;=1");
  BOOST_CHECK_EQUAL(p.host, "example.com");
  BOOST_CHECK_EQUAL(p.port, "80");
  BOOST_CHECK_EQUAL(p.port_value, 80);
  BOOST_CHECK_EQUAL(p.path, "/home/tim/test.txt");
  BOOST_CHECK_EQUAL(p.query, "");
  BOOST_CHECK_EQUAL(p.fragment, "");
}

BOOST_AUTO_TEST_CASE(special)
{
  parsed_url p;
  asioext::error_code ec;

  p = parse_url("file:///home/tim/test.txt", ec);
  BOOST_REQUIRE_MESSAGE(!ec, "ec:" << ec);
  BOOST_CHECK_EQUAL(p.scheme, "file");
  BOOST_CHECK_EQUAL(p.userinfo, "");
  BOOST_CHECK_EQUAL(p.host, "");
  BOOST_CHECK_EQUAL(p.port, "");
  BOOST_CHECK_EQUAL(p.port_value, 0);
  BOOST_CHECK_EQUAL(p.path, "/home/tim/test.txt");
  BOOST_CHECK_EQUAL(p.query, "");
  BOOST_CHECK_EQUAL(p.fragment, "");

  p = parse_url("file:/home/tim/test.txt", ec);
  BOOST_REQUIRE_MESSAGE(!ec, "ec:" << ec);
  BOOST_CHECK_EQUAL(p.scheme, "file");
  BOOST_CHECK_EQUAL(p.userinfo, "");
  BOOST_CHECK_EQUAL(p.host, "");
  BOOST_CHECK_EQUAL(p.port, "");
  BOOST_CHECK_EQUAL(p.port_value, 0);
  BOOST_CHECK_EQUAL(p.path, "/home/tim/test.txt");
  BOOST_CHECK_EQUAL(p.query, "");
  BOOST_CHECK_EQUAL(p.fragment, "");

  p = parse_url("javascript:alert(1)", ec);
  BOOST_REQUIRE_MESSAGE(!ec, "ec:" << ec);
  BOOST_CHECK_EQUAL(p.scheme, "javascript");
  BOOST_CHECK_EQUAL(p.userinfo, "");
  BOOST_CHECK_EQUAL(p.host, "");
  BOOST_CHECK_EQUAL(p.port, "");
  BOOST_CHECK_EQUAL(p.port_value, 0);
  BOOST_CHECK_EQUAL(p.path, "alert(1)");
  BOOST_CHECK_EQUAL(p.query, "");
  BOOST_CHECK_EQUAL(p.fragment, "");
}

BOOST_AUTO_TEST_SUITE_END()

ASIOEXT_NS_END
