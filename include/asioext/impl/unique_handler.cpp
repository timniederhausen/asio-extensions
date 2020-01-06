/// @copyright Copyright (c) 2019 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "asioext/unique_handler.hpp"
#include "asioext/exception.hpp"

#include "asioext/detail/throw_exception.hpp"

ASIOEXT_NS_BEGIN

namespace detail {

void throw_bad_function_call()
{
  bad_function_call ex;
  throw_exception(ex);
}

}

ASIOEXT_NS_END
