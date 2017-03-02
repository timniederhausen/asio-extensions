/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "asioext/scoped_file_handle.hpp"

#include "asioext/detail/posix_file_ops.hpp"
#include "asioext/detail/throw_error.hpp"

ASIOEXT_NS_BEGIN

scoped_file_handle::scoped_file_handle(const char* filename,
                                       open_flags flags,
                                       error_code& ec) ASIOEXT_NOEXCEPT
  : handle_(detail::posix_file_ops::open(filename, flags, ec))
{
  // ctor
}

#if defined(ASIOEXT_HAS_BOOST_FILESYSTEM)

scoped_file_handle::scoped_file_handle(const boost::filesystem::path& filename,
                                       open_flags flags,
                                       error_code& ec) ASIOEXT_NOEXCEPT
  : handle_(detail::posix_file_ops::open(filename.c_str(), flags, ec))
{
  // ctor
}

#endif

ASIOEXT_NS_END
