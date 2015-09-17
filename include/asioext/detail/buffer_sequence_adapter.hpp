/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_DETAIL_BUFFERSEQUENCEADAPTER_HPP
#define ASIOEXT_DETAIL_BUFFERSEQUENCEADAPTER_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
#pragma once
#endif

#include <asio/detail/buffer_sequence_adapter.hpp>

#include "asioext/detail/push_options.hpp"

ASIOEXT_NS_BEGIN

namespace detail {

using asio::detail::buffer_sequence_adapter;

}

ASIOEXT_NS_END

#include "asioext/detail/pop_options.hpp"

#if defined(ASIOEXT_HEADER_ONLY)
# include "asioext/detail/impl/win_file_ops.cpp"
#endif

#endif
