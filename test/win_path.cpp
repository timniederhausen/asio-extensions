#include "asioext/detail/win_path.hpp"

#include <boost/test/unit_test.hpp>

#include <string>

#include <windows.h>

ASIOEXT_NS_BEGIN

BOOST_AUTO_TEST_SUITE(asioext_win_path)

using asioext::detail::win_path;

BOOST_AUTO_TEST_CASE(empty)
{
  asioext::error_code ec;

  win_path p("", 0, ec);
  BOOST_REQUIRE_MESSAGE(!!ec, "ec: " << ec);
  BOOST_REQUIRE_EQUAL(ec.value(), ERROR_INVALID_PARAMETER);
}

BOOST_AUTO_TEST_CASE(simple)
{
  asioext::error_code ec;

  const char s1[] = "D:\\test\\abcdef.ghi";

  win_path p1(s1, std::strlen(s1), ec);
  BOOST_REQUIRE(!ec);
  BOOST_CHECK_EQUAL(p1.c_str(), static_cast<const wchar_t*>(L"D:\\test\\abcdef.ghi"));

  win_path p2(s1, std::size(s1) - 1, ec);
  BOOST_REQUIRE(!ec);
  BOOST_CHECK_EQUAL(p2.c_str(), static_cast<const wchar_t*>(L"D:\\test\\abcdef.ghi"));
}

BOOST_AUTO_TEST_CASE(error_reset)
{
  asioext::error_code ec;

  win_path p1("", 0, ec);
  BOOST_REQUIRE_MESSAGE(ec, "ec: " << ec);
  BOOST_REQUIRE_EQUAL(ec.value(), ERROR_INVALID_PARAMETER);

  const char s1[] = "hello.gni";

  win_path p2(s1, std::strlen(s1), ec);
  BOOST_REQUIRE_MESSAGE(!ec, "ec: " << ec);
}

BOOST_AUTO_TEST_CASE(large)
{
  asioext::error_code ec;

  const char s1[] = "D:\\test\\abcdef_abcdef_abcdef_abcdef_abcdef_abcdef_"
      "abcdef_abcdef_abcdef_abcdef_abcdef_abcdef_abcdef_abcdef_"
      "abcdef_abcdef_abcdef_abcdef_abcdef_abcdef_abcdef_abcdef_"
      "abcdef_abcdef_abcdef_abcdef_abcdef_abcdef_abcdef_abcdef_"
      "abcdef_abcdef_abcdef_abcdef_abcdef_abcdef_abcdef_.ghi";

  const wchar_t s2[] = L"D:\\test\\abcdef_abcdef_abcdef_abcdef_abcdef_abcdef_"
      "abcdef_abcdef_abcdef_abcdef_abcdef_abcdef_abcdef_abcdef_"
      "abcdef_abcdef_abcdef_abcdef_abcdef_abcdef_abcdef_abcdef_"
      "abcdef_abcdef_abcdef_abcdef_abcdef_abcdef_abcdef_abcdef_"
      "abcdef_abcdef_abcdef_abcdef_abcdef_abcdef_abcdef_.ghi";

  win_path p1(s1, std::strlen(s1), ec);
  BOOST_REQUIRE(!ec);
  BOOST_CHECK_EQUAL(p1.c_str(), static_cast<const wchar_t*>(s2));

  win_path p2(s1, std::size(s1) - 1, ec);
  BOOST_REQUIRE(!ec);
  BOOST_CHECK_EQUAL(p2.c_str(), static_cast<const wchar_t*>(s2));
}

BOOST_AUTO_TEST_SUITE_END()

ASIOEXT_NS_END
