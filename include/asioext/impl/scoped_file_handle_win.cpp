/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "asioext/detail/win_file_ops.hpp"
#include "asioext/detail/throw_error.hpp"

#include "asioext/scoped_file_handle.hpp"

ASIOEXT_NS_BEGIN

scoped_file_handle::scoped_file_handle(const char* filename, uint32_t flags)
{
  open(filename, flags);
}

scoped_file_handle::scoped_file_handle(const char* filename,
                                       uint32_t flags,
                                       error_code& ec) ASIOEXT_NOEXCEPT
{
  open(filename, flags, ec);
}

#if defined(ASIOEXT_HAS_BOOST_FILESYSTEM)

scoped_file_handle::scoped_file_handle(const boost::filesystem::path& filename,
                                       uint32_t flags)
{
  open(filename, flags);
}

scoped_file_handle::scoped_file_handle(const boost::filesystem::path& filename,
                                       uint32_t flags,
                                       error_code& ec) ASIOEXT_NOEXCEPT
  : handle_(detail::win_file_ops::open(filename.c_str(), flags, ec))
{
  // ctor
}

#endif

ASIOEXT_NS_END
