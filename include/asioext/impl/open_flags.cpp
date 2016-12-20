/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "asioext/open_flags.hpp"

ASIOEXT_NS_BEGIN

namespace open_flags {

bool is_valid(uint32_t flags)
{
  int creation_modes = !!(flags & create_new) +
                       !!(flags & create_always) +
                       !!(flags & open_existing) +
                       !!(flags & open_always) +
                       !!(flags & truncate_existing);

  if (creation_modes > 1)
    return false;

  const uint32_t unsupported_flags =
#if defined(ASIOEXT_WINDOWS)
    attribute_executable;
#else
    attribute_hidden;
#endif

  if (flags & unsupported_flags)
    return false;

  return true;
}

}

ASIOEXT_NS_END
