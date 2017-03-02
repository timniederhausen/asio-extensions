/// @copyright Copyright (c) 2016 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "asioext/open.hpp"

#include "asioext/detail/error_code.hpp"
#include "asioext/detail/throw_error.hpp"

#if defined(ASIOEXT_WINDOWS)
# include "asioext/detail/win_file_ops.hpp"
# if defined(ASIOEXT_WINDOWS_USE_UTF8_FILENAMES)
#  include "asioext/detail/win_path.hpp"
# endif
#else
# include "asioext/detail/posix_file_ops.hpp"
#endif

ASIOEXT_NS_BEGIN

file_handle open(const char* filename, open_flags flags)
{
  error_code ec;
  const file_handle h = open(filename, flags, ec);
  detail::throw_error(ec);
  return h;
}

file_handle open(const char* filename, open_flags flags,
                 error_code& ec) ASIOEXT_NOEXCEPT
{
#if defined(ASIOEXT_WINDOWS)
# if defined(ASIOEXT_WINDOWS_USE_UTF8_FILENAMES)
  detail::win_path p(filename, std::strlen(filename), ec);
  if (!ec)
    return detail::win_file_ops::open(p.c_str(), flags, ec);
  return file_handle();
# else
  return detail::win_file_ops::open(filename, flags, ec);
# endif
#else
  return detail::posix_file_ops::open(filename, flags, ec);
#endif
}

#if defined(ASIOEXT_WINDOWS)
file_handle open(const wchar_t* filename, open_flags flags)
{
  error_code ec;
  const file_handle h = open(filename, flags, ec);
  detail::throw_error(ec);
  return h;
}

file_handle open(const wchar_t* filename, open_flags flags,
                 error_code& ec) ASIOEXT_NOEXCEPT
{
  return detail::win_file_ops::open(filename, flags, ec);
}
#endif

#if defined(ASIOEXT_HAS_BOOST_FILESYSTEM)
file_handle open(const boost::filesystem::path& filename, open_flags flags)
{
  error_code ec;
  const file_handle h = open(filename, flags, ec);
  detail::throw_error(ec);
  return h;
}

file_handle open(const boost::filesystem::path& filename, open_flags flags,
                 error_code& ec) ASIOEXT_NOEXCEPT
{
#if defined(ASIOEXT_WINDOWS)
  return detail::win_file_ops::open(filename.c_str(), flags, ec);
#else
  return detail::posix_file_ops::open(filename.c_str(), flags, ec);
#endif
}
#endif

ASIOEXT_NS_END
