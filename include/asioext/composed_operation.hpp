/// @file
/// Declares the asioext::composed_operation utility class.
///
/// @copyright Copyright (c) 2016 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_COMPOSEDOPERATION_HPP
#define ASIOEXT_COMPOSEDOPERATION_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include "asioext/detail/asio_version.hpp"

#if defined(ASIOEXT_USE_BOOST_ASIO)
# define ASIOEXT_HANDLER_ALLOC_HELPERS_NS boost_asio_handler_alloc_helpers
# define ASIOEXT_HANDLER_CONT_HELPERS_NS boost_asio_handler_cont_helpers
# define ASIOEXT_HANDLER_INVOKE_HELPERS_NS boost_asio_handler_invoke_helpers
#else
# define ASIOEXT_HANDLER_ALLOC_HELPERS_NS asio_handler_alloc_helpers
# define ASIOEXT_HANDLER_CONT_HELPERS_NS asio_handler_cont_helpers
# define ASIOEXT_HANDLER_INVOKE_HELPERS_NS asio_handler_invoke_helpers
#endif

ASIOEXT_NS_BEGIN

/// @ingroup service
/// @brief Base class for composed operations.
///
/// This class template is intended to be used as a base class for custom
/// composed operation types that wrap a user-provided handler.
/// As such, it expects child classes to implement <code>operator()</code>.
///
/// All this class does is wrap the given handler object and provide
/// overloads for asio's hooks that forward to the real handler's hooks.
///
/// For a detailed description of `composed operations`, see
/// [async.reqmts.async.composed] inside the Networking TS.
///
/// Implemented hooks:
/// * asio_handler_allocate()
/// * asio_handler_deallocate()
/// * asio_handler_is_continuation()
/// * asio_handler_invoke()
///
/// On Asio 1.11.0+
/// * asio::associated_allocator<>
/// * asio::associated_executor<>
///
/// @note This type's <code>operator()</code> is executed by the
/// user-specified executor / invocation hook (see above).
/// It is not suitable for operations that are to be executed
/// in a service-provided context (e.g. a private io_service).
template <typename Handler>
class composed_operation
{
public:
  composed_operation(ASIOEXT_MOVE_ARG(Handler) handler)
    : handler_(ASIOEXT_MOVE_CAST(Handler)(handler))
  {
    // ctor
  }

#if defined(ASIOEXT_HAS_MOVE)
  composed_operation(const composed_operation& other)
    : handler_(ASIOEXT_MOVE_CAST(Handler)(other.handler_))
  {
    // ctor
  }

  composed_operation(composed_operation&& other)
    : handler_(ASIOEXT_MOVE_CAST(Handler)(other.handler_))
  {
    // ctor
  }
#endif

// TODO(tim): Make this protected.
//protected:
  Handler handler_;
};

#if !defined(ASIOEXT_IS_DOCUMENTATION)
template <typename Handler>
inline void* asio_handler_allocate(std::size_t size,
    composed_operation<Handler>* this_handler)
{
  return ASIOEXT_HANDLER_ALLOC_HELPERS_NS::allocate(
      size, this_handler->handler_);
}

template <typename Handler>
inline void asio_handler_deallocate(void* pointer, std::size_t size,
    composed_operation<Handler>* this_handler)
{
  ASIOEXT_HANDLER_ALLOC_HELPERS_NS::deallocate(
      pointer, size, this_handler->handler_);
}

template <typename Handler>
inline bool asio_handler_is_continuation(
    composed_operation<Handler>* this_handler)
{
  return ASIOEXT_HANDLER_CONT_HELPERS_NS::is_continuation(
      this_handler->handler_);
}

template <typename Function, typename Handler>
inline void asio_handler_invoke(Function& function,
                                composed_operation<Handler>* this_handler)
{
  ASIOEXT_HANDLER_INVOKE_HELPERS_NS::invoke(
      function, this_handler->handler_);
}

template <typename Function, typename Handler>
inline void asio_handler_invoke(const Function& function,
                                composed_operation<Handler>* this_handler)
{
  ASIOEXT_HANDLER_INVOKE_HELPERS_NS::invoke(
      function, this_handler->handler_);
}
#endif

ASIOEXT_NS_END

#if !defined(ASIOEXT_IS_DOCUMENTATION) && (ASIOEXT_ASIO_VERSION >= 101100)
# if defined(ASIOEXT_USE_BOOST_ASIO)
namespace boost {
# endif
namespace asio {

template <typename Handler, typename Allocator>
struct associated_allocator<asioext::composed_operation<Handler>, Allocator>
{
  typedef typename associated_allocator<Handler, Allocator>::type type;

  static type get(const asioext::composed_operation<Handler>& h,
                  const Allocator& a = Allocator()) ASIOEXT_NOEXCEPT
  {
    return associated_allocator<Handler, Allocator>::get(h.handler_, a);
  }
};

template <typename Handler, typename Executor>
struct associated_executor<asioext::composed_operation<Handler>, Executor>
{
  typedef typename associated_executor<Handler, Executor>::type type;

  static type get(const asioext::composed_operation<Handler>& h,
                  const Executor& ex = Executor()) ASIOEXT_NOEXCEPT
  {
    return associated_executor<Handler, Executor>::get(h.handler_, ex);
  }
};

}
# if defined(ASIOEXT_USE_BOOST_ASIO)
}
# endif
#endif

#endif
