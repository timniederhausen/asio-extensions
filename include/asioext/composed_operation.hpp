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
#include "asioext/detail/move_support.hpp"

#if defined(ASIOEXT_USE_BOOST_ASIO)
# include <boost/asio/detail/handler_alloc_helpers.hpp>
# include <boost/asio/detail/handler_cont_helpers.hpp>
# include <boost/asio/detail/handler_invoke_helpers.hpp>
# define ASIOEXT_HANDLER_ALLOC_HELPERS_NS boost_asio_handler_alloc_helpers
# define ASIOEXT_HANDLER_CONT_HELPERS_NS boost_asio_handler_cont_helpers
# define ASIOEXT_HANDLER_INVOKE_HELPERS_NS boost_asio_handler_invoke_helpers
#else
# include <asio/detail/handler_alloc_helpers.hpp>
# include <asio/detail/handler_cont_helpers.hpp>
# include <asio/detail/handler_invoke_helpers.hpp>
# define ASIOEXT_HANDLER_ALLOC_HELPERS_NS asio_handler_alloc_helpers
# define ASIOEXT_HANDLER_CONT_HELPERS_NS asio_handler_cont_helpers
# define ASIOEXT_HANDLER_INVOKE_HELPERS_NS asio_handler_invoke_helpers
#endif

ASIOEXT_NS_BEGIN

#if !defined(ASIOEXT_IS_DOCUMENTATION) && (ASIOEXT_ASIO_VERSION >= 101100)
template <typename Handler>
class composed_operation;

namespace detail {

template <typename>
struct associated_type_check
{
  typedef void type;
};

template <typename T, typename = void>
struct executor_wrapper_impl
{ };

template <typename T>
struct executor_wrapper_impl<T,
  typename associated_type_check<typename T::executor_type>::type>
{
  typedef typename T::executor_type executor_type;
  executor_type get_executor() const ASIOEXT_NOEXCEPT
  {
    return static_cast<const composed_operation<T>&>(*this).handler_.
        get_executor();
  }
};

template <typename T, typename = void>
struct allocator_wrapper_impl
{ };

template <typename T>
struct allocator_wrapper_impl<T,
  typename associated_type_check<typename T::allocator_type>::type>
{
  typedef typename T::allocator_type allocator_type;
  allocator_type get_allocator() const ASIOEXT_NOEXCEPT
  {
    return static_cast<const composed_operation<T>&>(*this).handler_.
        get_allocator();
  }
};

}
#endif

/// @ingroup core
/// @brief Base class for composed operations.
///
/// This class template is intended to be used as a base class for custom
/// composed operation types that wrap a user-provided handler.
/// As such, it expects child classes to implement <code>operator()</code>.
///
/// It wraps the given handler object and provides
/// overloads for asio's hooks that forward to the real handler's hooks
/// (if implemented).
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
/// * asio::associated_allocator<> support via
///   @c executor_type / @c get_executor
/// * asio::associated_executor<> support via
///   @c allocator_type / @c get_allocator
///
/// @note This type's <code>operator()</code> is executed by the
/// user-specified executor / invocation hook (see above).
/// It is not suitable for operations that are to be executed
/// in a service-provided context (e.g. a private io_service).
template <typename Handler>
class composed_operation
#if !defined(ASIOEXT_IS_DOCUMENTATION) && (ASIOEXT_ASIO_VERSION >= 101100)
  : public detail::executor_wrapper_impl<Handler>
  , public detail::allocator_wrapper_impl<Handler>
#endif
{
public:
  composed_operation(ASIOEXT_MOVE_ARG(Handler) handler)
    : handler_(ASIOEXT_MOVE_CAST(Handler)(handler))
  {
    // ctor
  }

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

#endif
