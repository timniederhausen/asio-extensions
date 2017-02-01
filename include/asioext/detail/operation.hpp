/// @copyright Copyright (c) 2017 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_DETAIL_OPERATION_HPP
#define ASIOEXT_DETAIL_OPERATION_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include "asioext/composed_operation.hpp"

#include "asioext/detail/asio_version.hpp"

#if defined(ASIOEXT_USE_BOOST_ASIO)
# include <boost/asio/io_service.hpp>
#else
# include <asio/io_service.hpp>
#endif

ASIOEXT_NS_BEGIN

namespace detail {

/// @brief Base class for custom operations.
///
/// This class template provides basic utilities for custom operations
/// that support user-supplied Handlers.
///
/// The operation class assumes that the the given Handler needs to be
/// executed on a specific io_service. It ensures that said io_service
/// doesn't run out of work until the handler was dispatched.
///
/// This class is intended for custom operations that are executed
/// on different io_service(s) than the user-supplied handler.
/// For example, a service that internally uses a separate io_service
/// object to do its work, still needs to execute the user's handler
/// inside the original io_service that owns the service object.
template <typename Handler>
class operation : public composed_operation<Handler>
{
public:
  operation(ASIOEXT_MOVE_ARG(Handler) handler, asio::io_service& executor)
    : composed_operation<Handler>(ASIOEXT_MOVE_CAST(Handler)(handler))
    , executor_(executor)
    , work_(executor)
  {
    // ctor
  }

  /// Get a reference to the assigned io_service.
  asio::io_service& get_executor() { return executor_; }

private:
  asio::io_service& executor_;
  asio::io_service::work work_;
};

// Operations aren't executed on the user's context
// so we need to ensure that the user's hooks / executors aren't used.

template <typename Function, typename Handler>
inline void asio_handler_invoke(Function& function,
                                operation<Handler>* this_handler)
{
  function();
}

template <typename Function, typename Handler>
inline void asio_handler_invoke(const Function& function,
                                operation<Handler>* this_handler)
{
  function();
}

}

ASIOEXT_NS_END

#endif
