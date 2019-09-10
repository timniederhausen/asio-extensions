/// @copyright Copyright (c) 2017 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_DETAIL_SERVICEBASE_HPP
#define ASIOEXT_DETAIL_SERVICEBASE_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#if defined(ASIOEXT_USE_BOOST_ASIO)
# include <boost/asio/io_context.hpp>
#else
# include <asio/io_context.hpp>
#endif

ASIOEXT_NS_BEGIN

namespace detail {

template <typename Service>
struct service_id : asio::execution_context::id
{};

template <typename Service>
class io_context_service_base : public asio::io_context::service
{
public:
  static service_id<Service> id;

  // Constructor.
  io_context_service_base(asio::io_context& owner)
    : asio::io_context::service(owner)
  {}
};

template <typename Service>
service_id<Service> io_context_service_base<Service>::id;

}

ASIOEXT_NS_END

#endif
