#include "asioext/detail/chrono.hpp"

#include <boost/test/unit_test.hpp>

#include <string>

ASIOEXT_NS_BEGIN

BOOST_AUTO_TEST_SUITE(asioext_chrono)

template <class Duration1, class Duration2>
void duration_cast_ok(const Duration1& d1, const Duration2& dur2)
{
  Duration2 actual;
  BOOST_REQUIRE(detail::safe_duration_cast(d1, actual));
  BOOST_CHECK_EQUAL(actual.count(), dur2.count());
}

template <class Duration1, class Duration2>
void duration_cast_fail(const Duration1& d1, const Duration2& dur2)
{
  Duration2 actual;
  BOOST_CHECK(!detail::safe_duration_cast(d1, actual));
}

BOOST_AUTO_TEST_CASE(safe_duration_cast)
{
  // Valid casts
  duration_cast_ok(chrono::duration<int64_t>(chrono::duration<int32_t>::min()),
                   chrono::duration<int32_t>::min());

  // Valid conversions
  duration_cast_ok(chrono::seconds(4), chrono::microseconds(4 * 1000 * 1000));
  duration_cast_ok(chrono::seconds(4 * 60 * 60), chrono::hours(4));
  duration_cast_ok(chrono::nanoseconds(4 * 1000000000ll * 60 * 60),
                   chrono::hours(4));

  // Overflow (cast)
  duration_cast_fail(chrono::duration<int64_t>::max(),
                     chrono::duration<int32_t>());
  duration_cast_fail(chrono::duration<int64_t>(chrono::duration<int32_t>::min()) -
                     chrono::duration<int64_t>(1),
                     chrono::duration<int32_t>());

  // Overflow (conversion)
  duration_cast_fail(chrono::duration_cast<chrono::hours>(
                        chrono::nanoseconds::max()) + chrono::hours(1),
                     chrono::nanoseconds());
}

BOOST_AUTO_TEST_CASE(time_fragment_values)
{
  typedef detail::time_fragment_values<
    chrono::microseconds, chrono::seconds, chrono::nanoseconds
  > test_type;
  typedef std::numeric_limits<chrono::microseconds::rep> limit_type;

  BOOST_CHECK_GE(sizeof(test_type::common_type),
                 sizeof(chrono::microseconds::rep));
  BOOST_CHECK_GE(sizeof(test_type::common_type),
                 sizeof(chrono::seconds::rep));
  BOOST_CHECK_GE(sizeof(test_type::common_type),
                 sizeof(chrono::nanoseconds::rep));

  BOOST_CHECK_EQUAL(limit_type::max() / 1000 / 1000, test_type::max_first());
  BOOST_CHECK_EQUAL(limit_type::min() / 1000 / 1000, test_type::min_first());

  static const chrono::microseconds::rep max_rem = limit_type::max() -
      (limit_type::max() / 1000 / 1000) * 1000 * 1000;
  static const chrono::microseconds::rep min_rem = limit_type::min() -
      (limit_type::min() / 1000 / 1000) * 1000 * 1000;

  BOOST_CHECK_EQUAL(max_rem * 1000, test_type::max_second());
  BOOST_CHECK_EQUAL((min_rem + 1000000) * 1000, test_type::min_second());

  // re-composition
  BOOST_CHECK_EQUAL(limit_type::max(),
                    test_type::max_first() * 1000 * 1000 +
                    test_type::max_second() / 1000);
  BOOST_CHECK_EQUAL(limit_type::min(),
                    test_type::min_first() * 1000 * 1000 +
                    test_type::min_second() / 1000 -
                    1000 * 1000);
}

template <class Duration, class FirstDuration, class SecondDuration>
void decompose_time_ok(const Duration& in, const FirstDuration& first,
                       const SecondDuration& second)
{
  FirstDuration actual_first;
  SecondDuration actual_second;
  BOOST_REQUIRE(detail::decompose_time(in, actual_first, actual_second));
  BOOST_CHECK_EQUAL(actual_first.count(), first.count());
  BOOST_CHECK_EQUAL(actual_second.count(), second.count());
}

template <class Duration, class FirstDuration, class SecondDuration>
void decompose_time_fail(const Duration& in, const FirstDuration& first,
                         const SecondDuration& second)
{
  FirstDuration actual_first;
  SecondDuration actual_second;
  BOOST_REQUIRE(!detail::decompose_time(in, actual_first, actual_second));
}

BOOST_AUTO_TEST_CASE(decompose_time)
{
  decompose_time_ok(chrono::seconds(123) + chrono::nanoseconds(456789),
                    chrono::seconds(123), chrono::nanoseconds(456789));

  typedef detail::time_fragment_values<
    chrono::nanoseconds, chrono::seconds, chrono::nanoseconds
  > test_values;
  decompose_time_ok(chrono::nanoseconds::max(),
                    chrono::seconds(test_values::max_first()),
                    chrono::nanoseconds(test_values::max_second()));
  decompose_time_ok(chrono::nanoseconds::min(),
                    chrono::seconds(test_values::min_first() - 1),
                    chrono::nanoseconds(test_values::min_second()));
}

template <class Duration, class FirstDuration, class SecondDuration>
void compose_time_ok(const Duration& in, const FirstDuration& first,
                       const SecondDuration& second)
{
  Duration actual;
  BOOST_REQUIRE(detail::compose_time(first, second, actual));
  BOOST_CHECK_EQUAL(actual.count(), in.count());
}

template <class Duration, class FirstDuration, class SecondDuration>
void compose_time_fail(const Duration& in, const FirstDuration& first,
                       const SecondDuration& second)
{
  Duration actual;
  BOOST_REQUIRE(!detail::compose_time(first, second, actual));
}

BOOST_AUTO_TEST_CASE(compose_time)
{
  compose_time_ok(chrono::seconds(123) + chrono::nanoseconds(456789),
                  chrono::seconds(123), chrono::nanoseconds(456789));

  typedef detail::time_fragment_values<
    chrono::nanoseconds, chrono::seconds, chrono::nanoseconds
  > test_values;

  compose_time_ok(chrono::nanoseconds::max(),
                  chrono::seconds(test_values::max_first()),
                  chrono::nanoseconds(test_values::max_second()));
  compose_time_ok(chrono::nanoseconds::min(),
                  chrono::seconds(test_values::min_first() - 1),
                  chrono::nanoseconds(test_values::min_second()));

  compose_time_fail(chrono::nanoseconds(),
                    chrono::seconds(test_values::max_first() + 1),
                    chrono::nanoseconds());

  compose_time_fail(chrono::nanoseconds(),
                    chrono::seconds(test_values::max_first()),
                    chrono::nanoseconds(test_values::max_second() + 1));

  compose_time_fail(chrono::nanoseconds(),
                    chrono::seconds(chrono::nanoseconds::max().count()),
                    chrono::nanoseconds());

  compose_time_fail(chrono::nanoseconds(),
                    chrono::seconds(test_values::min_first() - 1),
                    chrono::nanoseconds());

  compose_time_fail(chrono::nanoseconds(),
                    chrono::seconds(test_values::min_first() - 1),
                    chrono::nanoseconds(test_values::min_second() - 1));
}

#if defined(ASIOEXT_WINDOWS)
BOOST_AUTO_TEST_CASE(to_time_t)
{
  BOOST_CHECK_EQUAL(0, file_clock::to_time_t(file_time_type(
      file_clock::duration(116444736000000000ll))));
  BOOST_CHECK_EQUAL(3333, file_clock::to_time_t(file_time_type(
      file_clock::duration(116444736000000000ll + 33330000000ll))));

  const std::int64_t max = std::numeric_limits<std::int64_t>::max();
  BOOST_CHECK_EQUAL((max - 116444736000000000ll) / 10000000,
      file_clock::to_time_t(file_time_type(file_clock::duration(max))));
}

BOOST_AUTO_TEST_CASE(from_time_t)
{
  BOOST_CHECK_EQUAL(116444736000000000ll,
                    file_clock::from_time_t(0).time_since_epoch().count());
  BOOST_CHECK_EQUAL(116444736000000000ll + 33330000000ll,
                    file_clock::from_time_t(3333).time_since_epoch().count());

  const std::int64_t max = std::numeric_limits<std::int64_t>::max();
  const std::int64_t max_t = (max - 116444736000000000ll) / 10000000;
  BOOST_CHECK_EQUAL(max_t * 10000000 + 116444736000000000ll,
                    file_clock::from_time_t(max_t).time_since_epoch().count());
}
#endif

BOOST_AUTO_TEST_SUITE_END()

ASIOEXT_NS_END
