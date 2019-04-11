/// @copyright Copyright (c) 2016 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "asioext/open.hpp"
#include "asioext/error_code.hpp"

#include "asioext/detail/throw_error.hpp"
#include "asioext/detail/move_support.hpp"

#if defined(ASIOEXT_WINDOWS)
# include "asioext/detail/win_file_ops.hpp"
#else
# include "asioext/detail/posix_file_ops.hpp"
#endif

ASIOEXT_NS_BEGIN

unique_file_handle open(const char* filename, const open_args& args)
{
  error_code ec;
  unique_file_handle h = open(filename, args, ec);
  detail::throw_error(ec, "open");
  return h;
}

unique_file_handle open(const char* filename, const open_args& args,
                        error_code& ec) ASIOEXT_NOEXCEPT
{
#if defined(ASIOEXT_WINDOWS)
  return unique_file_handle(detail::win_file_ops::open(filename, args, ec));
#else
  return unique_file_handle(detail::posix_file_ops::open(filename, args, ec));
#endif
}

#if defined(ASIOEXT_WINDOWS)
unique_file_handle open(const wchar_t* filename, const open_args& args)
{
  error_code ec;
  const detail::win_file_ops::handle_type h =
      detail::win_file_ops::open(filename, args, ec);
  detail::throw_error(ec, "open");
  return unique_file_handle(h);
}

unique_file_handle open(const wchar_t* filename, const open_args& args,
                        error_code& ec) ASIOEXT_NOEXCEPT
{
  return unique_file_handle(detail::win_file_ops::open(filename, args, ec));
}
#endif

#if defined(ASIOEXT_HAS_BOOST_FILESYSTEM)
unique_file_handle open(const boost::filesystem::path& filename,
                        const open_args& args)
{
  error_code ec;
  unique_file_handle h = open(filename, args, ec);
  detail::throw_error(ec, "open");
  return h;
}

unique_file_handle open(const boost::filesystem::path& filename,
                        const open_args& args, error_code& ec) ASIOEXT_NOEXCEPT
{
#if defined(ASIOEXT_WINDOWS)
  return unique_file_handle(detail::win_file_ops::open(
      filename.c_str(), args, ec));
#else
  return unique_file_handle(detail::posix_file_ops::open(
      filename.c_str(), args, ec));
#endif
}
#endif

ASIOEXT_NS_END
