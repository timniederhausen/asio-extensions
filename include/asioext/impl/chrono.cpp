/// @copyright Copyright (c) 2017 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#include "asioext/chrono.hpp"
#include "asioext/error_code.hpp"

#include "asioext/detail/throw_error.hpp"

#if defined(ASIOEXT_WINDOWS)
# include <windows.h>
#else
# include <time.h>
# include <sys/time.h>
#endif

ASIOEXT_NS_BEGIN

file_clock::time_point file_clock::now() ASIOEXT_NOEXCEPT
{
#if defined(ASIOEXT_WINDOWS)
  FILETIME ft;
# if (_WIN32_WINNT >= _WIN32_WINNT_WIN8) && \
     WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
  GetSystemTimePreciseAsFileTime(&ft);
# else
  GetSystemTimeAsFileTime(&ft);
# endif
  return time_point(duration((static_cast<int64_t>(ft.dwHighDateTime) << 32) |
                             static_cast<int64_t>(ft.dwLowDateTime)));
#else
# if defined(CLOCK_REALTIME)
  struct timespec ts;
  if (0 != clock_gettime(CLOCK_REALTIME, &ts)) {
    detail::throw_error(error_code(errno, system_category()),
                        "file_clock::now");
  }
  return time_point(chrono::seconds(ts.tv_sec) +
                    chrono::nanoseconds(ts.tv_nsec));
# else
  struct timeval tv;
  gettimeofday(&tv, 0);
  return time_point(chrono::seconds(tv.tv_sec) +
                    chrono::microseconds(tv.tv_usec));
# endif
#endif
}

ASIOEXT_NS_END
