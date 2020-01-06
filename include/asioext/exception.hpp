/// @file
/// Defines custom exceptions used by this library.
///
/// @copyright Copyright (c) 2019 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_EXCEPTION_HPP
#define ASIOEXT_EXCEPTION_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include <stdexcept>

ASIOEXT_NS_BEGIN

/// @brief Thrown when an empty callable container is invoked
///
/// An exception of this type is thrown when an empty callable container,
/// such as a @ref asioext::unique_handler is invoked.
class bad_function_call : public std::runtime_error
{
public:
  bad_function_call()
    : std::runtime_error("call to empty callable container")
  {}
};

ASIOEXT_NS_END

#endif
