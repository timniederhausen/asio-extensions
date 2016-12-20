#include "asioext/open_flags.hpp"

#include <boost/test/unit_test.hpp>

#if defined(ASIOEXT_USE_BOOST_ASIO)
namespace asio = asioext::asio;
#endif

BOOST_AUTO_TEST_SUITE(asioext_open_flags)

BOOST_AUTO_TEST_CASE(simple)
{
  using namespace asioext::open_flags;

  BOOST_CHECK(is_valid(access_write | access_read));
  BOOST_CHECK(is_valid(access_write | open_always));
  BOOST_CHECK(is_valid(access_read | open_always));
}

BOOST_AUTO_TEST_CASE(platform_dependent)
{
  using namespace asioext::open_flags;

#if defined(ASIOEXT_WINDOWS)
  BOOST_CHECK(is_valid(create_always | attribute_hidden));
  BOOST_CHECK(!is_valid(create_always | attribute_executable));
#else
  BOOST_CHECK(!is_valid(create_always | attribute_hidden));
  BOOST_CHECK(is_valid(create_always | attribute_executable));
#endif
}

BOOST_AUTO_TEST_SUITE_END()
