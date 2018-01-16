/// @copyright Copyright (c) 2016 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "asioext/standard_streams.hpp"

#include "asioext/detail/throw_error.hpp"

#if defined(ASIOEXT_WINDOWS)
# include "asioext/detail/win_file_ops.hpp"
#else
# include "asioext/detail/posix_file_ops.hpp"
#endif

ASIOEXT_NS_BEGIN

file_handle get_stdin()
{
  error_code ec;
  const file_handle h = get_stdin(ec);
  return h;
}

file_handle get_stdin(error_code& ec) ASIOEXT_NOEXCEPT
{
#if defined(ASIOEXT_WINDOWS)
  return detail::win_file_ops::get_stdin(ec);
#else
  return detail::posix_file_ops::get_stdin(ec);
#endif
}

file_handle get_stdout()
{
  error_code ec;
  const file_handle h = get_stdout(ec);
  return h;
}

file_handle get_stdout(error_code& ec) ASIOEXT_NOEXCEPT
{
#if defined(ASIOEXT_WINDOWS)
  return detail::win_file_ops::get_stdout(ec);
#else
  return detail::posix_file_ops::get_stdout(ec);
#endif
}

file_handle get_stderr()
{
  error_code ec;
  const file_handle h = get_stderr(ec);
  return h;
}

file_handle get_stderr(error_code& ec) ASIOEXT_NOEXCEPT
{
#if defined(ASIOEXT_WINDOWS)
  return detail::win_file_ops::get_stderr(ec);
#else
  return detail::posix_file_ops::get_stderr(ec);
#endif
}

ASIOEXT_NS_END
