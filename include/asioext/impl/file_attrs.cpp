/// @copyright Copyright (c) 2017 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#include "asioext/file_attrs.hpp"

ASIOEXT_NS_BEGIN

bool is_valid(file_attrs flags)
{
  int count = int((flags & file_attrs::add_attrs) != file_attrs::none) +
              int((flags & file_attrs::remove_attrs) != file_attrs::none);
  if (count > 1)
    return false;

  return true;
}

ASIOEXT_NS_END
