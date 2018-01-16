/// @copyright Copyright (c) 2017 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_DETAIL_CHRONO_HPP
#define ASIOEXT_DETAIL_CHRONO_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include "asioext/chrono.hpp"

#include "asioext/detail/cstdint.hpp"

#include <ctime>

ASIOEXT_NS_BEGIN

namespace detail {

template <class RepIn, class PeriodIn, class RepOut, class PeriodOut>
ASIOEXT_CONSTEXPR14 bool safe_duration_cast(
    const chrono::duration<RepIn, PeriodIn>& in,
    chrono::duration<RepOut, PeriodOut>& out) ASIOEXT_NOEXCEPT;

template <class Duration, class FirstDuration, class SecondDuration>
bool decompose_time(const Duration& in, FirstDuration& first,
                    SecondDuration& second) ASIOEXT_NOEXCEPT;

template <class Duration, class FirstDuration, class SecondDuration>
bool compose_time(const FirstDuration& first, const SecondDuration& second,
                  Duration& out) ASIOEXT_NOEXCEPT;

}

ASIOEXT_NS_END

#include "asioext/detail/impl/chrono.hpp"

#endif
