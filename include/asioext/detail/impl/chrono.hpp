/// @copyright Copyright (c) 2017 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_DETAIL_IMPL_CHRONO_HPP
#define ASIOEXT_DETAIL_IMPL_CHRONO_HPP

#include <limits>

ASIOEXT_NS_BEGIN

namespace detail {

template <class RepIn, class PeriodIn, class RepOut, class PeriodOut>
ASIOEXT_CONSTEXPR14 bool safe_duration_cast(
    const chrono::duration<RepIn, PeriodIn>& in,
    chrono::duration<RepOut, PeriodOut>& out)
{
  typedef typename std::common_type<RepIn, RepOut, intmax_t>::type
      conv_type;
#if defined(ASIOEXT_USE_BOOST_CHRONO)
  typedef boost::ratio_divide<PeriodIn, PeriodOut> result_ratio;
#else
  typedef std::ratio_divide<PeriodIn, PeriodOut> result_ratio;
#endif
  typedef std::numeric_limits<conv_type> conv_limit;
  typedef std::numeric_limits<RepOut> result_limit;

  if (result_ratio::num == 1 && result_ratio::den == 1) {
    if (in.count() >= (result_limit::min)() &&
        in.count() <= (result_limit::max)()) {
      out = chrono::duration<RepOut, PeriodOut>(static_cast<RepOut>(
          in.count()));
      return true;
    }
  } else if (result_ratio::num != 1 && result_ratio::den == 1) {
    if (in.count() >= (result_limit::min)() / result_ratio::num &&
        in.count() <= (result_limit::max)() / result_ratio::num) {
      out = chrono::duration<RepOut, PeriodOut>(static_cast<RepOut>(
          static_cast<conv_type>(in.count()) *
          static_cast<conv_type>(result_ratio::num)));
      return true;
    }
  } else if (result_ratio::num == 1 && result_ratio::den != 1) {
    const conv_type temp = static_cast<conv_type>(in.count()) /
                           static_cast<conv_type>(result_ratio::den);
    if (temp  >= (result_limit::min)() && temp  <= (result_limit::max)()) {
      out = chrono::duration<RepOut, PeriodOut>(static_cast<RepOut>(temp));
      return true;
    }
  } else {
    if (in.count() < (conv_limit::min)() / result_ratio::num &&
        in.count() > (conv_limit::max)() / result_ratio::num)
      return false;
    const conv_type temp = static_cast<conv_type>(in.count()) *
                           static_cast<conv_type>(result_ratio::num) /
                           static_cast<conv_type>(result_ratio::den);
    if (temp  >= (result_limit::min)() && temp  <= (result_limit::max)()) {
      out = chrono::duration<RepOut, PeriodOut>(static_cast<RepOut>(temp));
      return true;
    }
  }

  return false;
}

template <class Duration, class FirstDuration, class SecondDuration>
struct time_fragment_values
{
  typedef typename std::common_type<
    typename Duration::rep,
    typename FirstDuration::rep,
    typename SecondDuration::rep,
    intmax_t
  >::type common_type;

  static ASIOEXT_CONSTEXPR typename FirstDuration::rep first_rep_min()
  { return (std::numeric_limits<typename FirstDuration::rep>::min)(); }

  static ASIOEXT_CONSTEXPR typename SecondDuration::rep second_rep_min()
  { return (std::numeric_limits<typename SecondDuration::rep>::min)(); }

  static ASIOEXT_CONSTEXPR common_type max_first()
  { return chrono::duration_cast<FirstDuration>((Duration::max)()).count(); }

  static ASIOEXT_CONSTEXPR common_type min_first()
  { return chrono::duration_cast<FirstDuration>((Duration::min)()).count(); }

  static ASIOEXT_CONSTEXPR common_type max_second()
  {
    return chrono::duration_cast<SecondDuration>(
        (Duration::max)() - FirstDuration(max_first())).count();
  }

  static ASIOEXT_CONSTEXPR common_type min_second()
  {
    // For negative |first|s, we use (first + 1f) - (1f - |second|)
    // so the smallest |second| we can accept for |min_f - 1| is
    // 1f + clock_min - min_f
    return chrono::duration_cast<SecondDuration>(
        (Duration::min)() - FirstDuration(min_first()) +
        FirstDuration(1)).count();
  }
};

template <class Duration, class FirstDuration, class SecondDuration>
bool decompose_time(const Duration& in, FirstDuration& first,
                    SecondDuration& second)
{
  typedef time_fragment_values<Duration, FirstDuration, SecondDuration>
      values_type;

  // Use the largest type we might need during computation.
  chrono::duration<typename values_type::common_type,
                   typename FirstDuration::period> first_temp;
  chrono::duration<typename values_type::common_type,
                   typename SecondDuration::period> second_temp;

  if (!safe_duration_cast(in, first_temp))
    return false;
  if (!safe_duration_cast(in - first_temp, second_temp))
    return false;

  // Ensure |second_temp| is not negative
  if (second_temp.count() < 0) {
    if (first_temp.count() > values_type::min_first() ||
        (values_type::first_rep_min() != values_type::min_first() &&
         first_temp.count() == values_type::min_first())) {
      first_temp -= FirstDuration(1);
      second_temp += FirstDuration(1);
    } else {
      return false;
    }
  }

  return safe_duration_cast(first_temp, first) &&
         safe_duration_cast(second_temp, second);
}

template <class Duration, class FirstDuration, class SecondDuration>
bool compose_time(const FirstDuration& first, const SecondDuration& second,
                  Duration& out)
{
  typedef time_fragment_values<Duration, FirstDuration, SecondDuration>
      values_type;

  if (first >= FirstDuration::zero()) {
    if (first.count() < values_type::max_first() ||
        (first.count() == values_type::max_first() &&
         second.count() <= values_type::max_second())) {
      out = Duration(first + second);
      return true;
    }
    return false;
  }

  if (values_type::first_rep_min() != values_type::min_first() &&
      first.count() == values_type::min_first() - 1) {
    if (second.count() < values_type::min_second())
      return false;
  } else if (first.count() < values_type::min_second()) {
    return false;
  }

  if (second == SecondDuration::zero()) {
    out = Duration(first);
    return true;
  }

  const Duration adjust = chrono::duration_cast<Duration>(
      FirstDuration(1) - second);
  out = Duration(first + FirstDuration(1)) - adjust;
  return true;
}

}

ASIOEXT_NS_END

#endif
