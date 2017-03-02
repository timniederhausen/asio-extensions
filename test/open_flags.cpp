#include "asioext/open_flags.hpp"

#include <boost/test/unit_test.hpp>

ASIOEXT_NS_BEGIN

BOOST_AUTO_TEST_SUITE(asioext_open_flags)

BOOST_AUTO_TEST_CASE(simple)
{
  BOOST_CHECK(is_valid(open_flags::access_write | open_flags::access_read));
  BOOST_CHECK(is_valid(open_flags::access_write | open_flags::open_always));
  BOOST_CHECK(is_valid(open_flags::access_read | open_flags::open_always));
}

BOOST_AUTO_TEST_SUITE_END()

ASIOEXT_NS_END
