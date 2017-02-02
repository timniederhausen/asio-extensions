#include "asioext/open_flags.hpp"

#include <boost/test/unit_test.hpp>

ASIOEXT_NS_BEGIN

BOOST_AUTO_TEST_SUITE(asioext_open_flags)

BOOST_AUTO_TEST_CASE(simple)
{
  BOOST_CHECK(are_open_flags_valid(access_write | access_read));
  BOOST_CHECK(are_open_flags_valid(access_write | open_always));
  BOOST_CHECK(are_open_flags_valid(access_read | open_always));
}

BOOST_AUTO_TEST_CASE(platform_dependent)
{
#if defined(ASIOEXT_WINDOWS)
  BOOST_CHECK(are_open_flags_valid(create_always | attribute_hidden));
  BOOST_CHECK(!are_open_flags_valid(create_always | attribute_executable));
#else
  BOOST_CHECK(!are_open_flags_valid(create_always | attribute_hidden));
  BOOST_CHECK(are_open_flags_valid(create_always | attribute_executable));
#endif
}

BOOST_AUTO_TEST_SUITE_END()

ASIOEXT_NS_END
