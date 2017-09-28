/// @copyright Copyright (c) 2017 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_CHRONO_HPP
#define ASIOEXT_CHRONO_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#if defined(ASIOEXT_USE_BOOST_CHRONO)
# include <boost/chrono.hpp>
#else
# include <chrono>
#endif

#include <ctime>

ASIOEXT_NS_BEGIN

#if defined(ASIOEXT_IS_DOCUMENTATION)
namespace chrono = user_chosen;
#elif defined(ASIOEXT_USE_BOOST_CHRONO)
namespace chrono = boost::chrono;
#else
namespace chrono = std::chrono;
#endif

/// @ingroup files
/// @brief Special clock for filesystem time points.
///
/// Special clock with the same epoch and accuracy the filesystem
/// uses for file times.
///
/// Filesystem time points are not necessarily
/// represented in system time, hence the need for a separate clock.
///
/// @see http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0355r3.html
class file_clock
{
public:
#if defined(ASIOEXT_IS_DOCUMENTATION)
  /// @brief Duration capable of representing file times
  typedef implementation_defined duration;
#elif defined(ASIOEXT_WINDOWS)
  // File times are in 100ns ticks.
  // https://msdn.microsoft.com/en-us/library/windows/desktop/ms724290(v=vs.85).aspx
  static const uint64_t ticks_per_second = 10000000;
  static const uint64_t epoch_difference_secs =
      ((1970 - 1601) * 365 + 3 * 24 + 17) * 86400ull;

# if defined(ASIOEXT_USE_BOOST_CHRONO)
  typedef chrono::duration<int64_t, boost::ratio<1, 10000000> > duration;
# else
  typedef chrono::duration<int64_t, std::ratio<1, 10000000> > duration;
# endif
#else
  typedef chrono::nanoseconds duration;
#endif
  typedef duration::rep rep;
  typedef duration::period period;
  typedef chrono::time_point<file_clock> time_point;

#if defined(ASIOEXT_IS_DOCUMENTATION)
  static constexpr bool is_steady = implementation_defined;
#else
  static const bool is_steady = false;
#endif

  ASIOEXT_DECL static time_point now() ASIOEXT_NOEXCEPT;

#if defined(ASIOEXT_WINDOWS)
  static std::time_t to_time_t(const time_point& t)
  {
    return static_cast<std::time_t>(chrono::duration_cast<chrono::seconds>(
        t.time_since_epoch() - chrono::seconds(epoch_difference_secs)).count());
  }

  static time_point from_time_t(std::time_t t)
  {
    return time_point(chrono::duration_cast<duration>(
        chrono::seconds(t) + chrono::seconds(epoch_difference_secs)));
  }
#else
  static std::time_t to_time_t(const time_point& t)
  {
    return static_cast<std::time_t>(chrono::duration_cast<chrono::seconds>(
        t.time_since_epoch()).count());
  }

  static time_point from_time_t(std::time_t t)
  {
    return time_point(chrono::duration_cast<duration>(chrono::seconds(t)));
  }
#endif
};

/// @ingroup files
/// @brief Representation of a file time (e.g. mtime)
typedef chrono::time_point<file_clock> file_time_type;

ASIOEXT_NS_END

#if defined(ASIOEXT_HEADER_ONLY)
# include "asioext/impl/chrono.cpp"
#endif

#endif
