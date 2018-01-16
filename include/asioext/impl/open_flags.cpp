/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "asioext/open_flags.hpp"

ASIOEXT_NS_BEGIN

bool is_valid(open_flags flags) ASIOEXT_NOEXCEPT
{
  int count = ((flags & open_flags::create_new) != open_flags::none) +
              ((flags & open_flags::create_always) != open_flags::none) +
              ((flags & open_flags::open_existing) != open_flags::none) +
              ((flags & open_flags::open_always) != open_flags::none) +
              ((flags & open_flags::truncate_existing) != open_flags::none);
  if (count > 1)
    return false;

  return true;
}

ASIOEXT_NS_END
