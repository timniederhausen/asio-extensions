/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_IMPL_OPENARGS_HPP
#define ASIOEXT_IMPL_OPENARGS_HPP

#if defined(ASIOEXT_WINDOWS)
# include "asioext/detail/win_file_ops.hpp"
#else
# include "asioext/detail/posix_file_ops.hpp"
#endif

ASIOEXT_NS_BEGIN

inline ASIOEXT_CONSTEXPR open_args::open_args() ASIOEXT_NOEXCEPT
#if defined(ASIOEXT_WINDOWS)
  : creation_disposition_(0xffffffff)
  , desired_access_(0)
  , share_mode_(0)
  , attrs_(0)
  , flags_(0)
#else
  : native_flags_(-1)
  , mode_(0)
  , attrs_(0)
#endif
{
  // ctor
}

inline open_args::open_args(open_flags flags, file_perms perms,
                            file_attrs attrs) ASIOEXT_NOEXCEPT
{
  if (asioext::is_valid(flags)) {
#if defined(ASIOEXT_WINDOWS)
    detail::win_file_ops::parse_open_flags(flags, perms, attrs, *this);
#else
    native_flags_ = detail::posix_file_ops::parse_open_flags(flags);
    mode_ = static_cast<uint32_t>(perms & file_perms::all);
    attrs_ = detail::posix_file_ops::file_attrs_to_native(attrs);
#endif
  } else {
#if defined(ASIOEXT_WINDOWS)
    creation_disposition_ = 0xffffffff;
#else
    native_flags_ = -1;
#endif
  }
}

ASIOEXT_NS_END

#endif
