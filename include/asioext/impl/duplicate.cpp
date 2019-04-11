/// @copyright Copyright (c) 2017 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#include "asioext/duplicate.hpp"

#include "asioext/detail/throw_error.hpp"

#if defined(ASIOEXT_WINDOWS)
# include "asioext/detail/win_file_ops.hpp"
#else
# include "asioext/detail/posix_file_ops.hpp"
#endif

ASIOEXT_NS_BEGIN

unique_file_handle duplicate(file_handle handle)
{
  error_code ec;
  unique_file_handle h = duplicate(handle, ec);
  detail::throw_error(ec, "duplicate");
  return h;
}

unique_file_handle duplicate(file_handle handle,
                             error_code& ec) ASIOEXT_NOEXCEPT
{
#if defined(ASIOEXT_WINDOWS)
  return unique_file_handle(detail::win_file_ops::duplicate(
      handle.native_handle(), ec));
#else
  return unique_file_handle(detail::posix_file_ops::duplicate(
      handle.native_handle(), ec));
#endif
}

ASIOEXT_NS_END
