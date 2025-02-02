/// @file
/// Defines the io_object_holder helper class.
///
/// @copyright Copyright (c) 2024 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)
#ifndef ASIOEXT_EXECUTIONCONTEXT_HPP
#define ASIOEXT_EXECUTIONCONTEXT_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#if defined(ASIOEXT_USE_BOOST_ASIO)
# include <boost/asio/execution_context.hpp>
#else
# include <asio/execution_context.hpp>
#endif

ASIOEXT_NS_BEGIN

template <typename T>
concept execution_context = std::is_convertible_v<T&, asio::execution_context&>;

ASIOEXT_NS_END

#endif
