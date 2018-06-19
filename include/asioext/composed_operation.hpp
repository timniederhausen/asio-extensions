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

#if (ASIOEXT_ASIO_VERSION >= 101100)
# if defined(ASIOEXT_USE_BOOST_ASIO)
#  include <boost/asio/associated_allocator.hpp>
#  include <boost/asio/associated_executor.hpp>
# else
#  include <asio/associated_allocator.hpp>
#  include <asio/associated_executor.hpp>
# endif
#endif

ASIOEXT_NS_BEGIN

namespace detail {

template <typename Handler, typename Operation>
class composed_op
{
#if !defined(ASIOEXT_IS_DOCUMENTATION) && (ASIOEXT_ASIO_VERSION >= 101100)
  template <typename T, typename Executor>
  friend struct asio::associated_allocator;

  template <typename T, typename Allocator>
  friend struct asio::associated_executor;
#endif

  friend void* asio_handler_allocate(std::size_t size,
                                     composed_op* this_handler)
  {
    return ASIOEXT_HANDLER_ALLOC_HELPERS_NS::allocate(
        size, this_handler->handler_);
  }

  friend void asio_handler_deallocate(void* pointer, std::size_t size,
                                      composed_op* this_handler)
  {
    ASIOEXT_HANDLER_ALLOC_HELPERS_NS::deallocate(
        pointer, size, this_handler->handler_);
  }

  friend bool asio_handler_is_continuation(composed_op* this_handler)
  {
    return ASIOEXT_HANDLER_CONT_HELPERS_NS::is_continuation(
        this_handler->handler_);
  }

  template <typename Function>
  friend void asio_handler_invoke(Function& function,
                                  composed_op* this_handler)
  {
    ASIOEXT_HANDLER_INVOKE_HELPERS_NS::invoke(
        function, this_handler->handler_);
  }

  template <typename Function>
  friend void asio_handler_invoke(const Function& function,
                                  composed_op* this_handler)
  {
    ASIOEXT_HANDLER_INVOKE_HELPERS_NS::invoke(
        function, this_handler->handler_);
  }

  Handler handler_;
  Operation op_;

public:
  template <typename Handler2, typename Operation2>
  explicit composed_op(Handler2&& handler, Operation2&& op)
    : handler_(std::forward<Handler2>(handler))
    , op_(std::forward<Operation2>(op))
  {
    // ctor
  }

  template <typename... Args>
  void operator()(Args&&... args)
    ASIOEXT_NOEXCEPT_IF(noexcept(op_(ASIOEXT_MOVE_CAST(Handler)(std::declval<Handler>()),
                                     std::forward<Args>(args)...)))
  {
    op_(ASIOEXT_MOVE_CAST(Handler)(handler_), std::forward<Args>(args)...);
  }
};

}

/// @ingroup core
/// @brief (Deprecated.) Base class for composed operations.
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
/// * @ref asio::asio_handler_allocate()
/// * @ref asio::asio_handler_deallocate()
/// * @ref asio::asio_handler_is_continuation()
/// * @ref asio::asio_handler_invoke()
///
/// This class does **not** provide support for:
///
/// * @ref asio::associated_allocator<> (Asio 1.11.0+)
/// * @ref asio::associated_executor<> (Asio 1.11.0+)
///
/// The user is required to manually specialize these templates in the asio namespace.
///
/// <b>Note:</b> This is one of the reasons why @ref make_composed_operation is
/// preferred.
///
/// Example specializations:
/// @code
/// namespace asio {
///
/// template <typename Handler, typename Allocator>
/// struct associated_allocator<myns::my_operation<Handler>, Allocator>
/// {
///   typedef typename associated_allocator<Handler, Allocator>::type type;
///
///   static type get(const myns::my_operation<Handler>& h,
///                   const Allocator& a = Allocator()) ASIOEXT_NOEXCEPT
///   {
///     return associated_allocator<Handler, Allocator>::get(h.handler_, a);
///   }
/// };
///
/// template <typename Handler, typename Executor>
/// struct associated_executor<myns::my_operation<Handler>, Executor>
/// {
///   typedef typename associated_executor<Handler, Executor>::type type;
///
///   static type get(const myns::my_operation<Handler>& h,
///                   const Executor& ex = Executor()) ASIOEXT_NOEXCEPT
///   {
///     return associated_executor<Handler, Executor>::get(h.handler_, ex);
///   }
/// };
///
/// }
/// @endcode
///
/// @note Composed operations are executed by the original handler's
/// user-specified executor / invocation hook (see above). <br>
/// It is therefore not suitable for cases where your custom
/// operation needs to execute in a specific way (e.g. when
/// simulating an asynchronous operation using a private threadpool).
template <typename Handler>
class composed_operation
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

#if defined(ASIOEXT_IS_DOCUMENTATION)
/// @ingroup core
/// @brief Adapt a function object to the ComposedOperation requirements.
///
/// This function binds the given function object to the specified handler,
/// creating a new @c Handler that retains the original handler's hooks/
/// customization points but forwards calls to @c op. A rvalue-reference
/// to the original handler is passed as first argument to @c op.
///
/// @c op will be invoked by the following expression:
/// @code
/// op(std::move(handler), std::forward<Args>(args)...)
/// @endcode
/// where @c args are the arguments that the resulting handler
/// has been called with.
///
/// The returned wrapper support the following customization points:
/// * @ref asio::asio_handler_allocate()
/// * @ref asio::asio_handler_deallocate()
/// * @ref asio::asio_handler_is_continuation()
/// * @ref asio::asio_handler_invoke()
/// * @ref asio::associated_allocator<> (for Asio 1.11.0+)
/// * @ref asio::associated_executor<> (for Asio 1.11.0+)
///
/// @param handler The @c Handler object whose hooks you wish to use.
/// @param op The function object that you wish to bind to the given handler.
/// @returns A new @c Handler that retains the original handler's hooks/
/// customization points but forwards invocations to `op`.
///
/// @par Example
/// @code
/// template <typename AsyncReadStream, typename AsyncWriteStream,
///           typename MutableBufferSequence, typename Handler>
/// asioext::async_result_t<Handler, void (asioext::error_code, std::size_t)>
/// async_copy(AsyncReadStream& from, AsyncWriteStream& to,
///            const MutableBufferSequence& buffers, Handler&& handler)
/// {
///   asioext::async_completion<
///       Handler, void (asioext::error_code, std::size_t)> init(handler);
///   auto op = [&to, buffers] (auto&& handler, asioext::error_code ec,
///                             std::size_t size) {
///     if (!ec)
///       asio::async_write(to, buffers, std::move(handler));
///     else
///       handler(ec, size);
///   };
///   asio::async_read(from, buffers,
///       asioext::make_composed_operation(std::move(init.completion_handler),
///                                        std::move(op)));
///   return init.result.get();
/// }
/// @endcode
///
/// @note Composed operations are executed by the original handler's
/// user-specified executor / invocation hook (see above). <br>
/// It is therefore not suitable for cases where your custom
/// operation needs to execute in a specific way (e.g. when
/// simulating an asynchronous operation using a private threadpool).
template <typename Handler, typename Operation>
implementation_defined make_composed_operation(Handler&& handler,
                                               Operation&& op);
#else
template <typename Handler, typename Operation>
detail::composed_op<
    typename std::decay<Handler>::type,
    typename std::decay<Operation>::type
> make_composed_operation(Handler&& handler, Operation&& op)
{
  return detail::composed_op<
    typename std::decay<Handler>::type,
    typename std::decay<Operation>::type
  >(std::forward<Handler>(handler), std::forward<Operation>(op));
}
#endif

ASIOEXT_NS_END

#if !defined(ASIOEXT_IS_DOCUMENTATION) && (ASIOEXT_ASIO_VERSION >= 101100)
# if defined(ASIOEXT_USE_BOOST_ASIO)
namespace boost {
# endif
namespace asio {

template <typename Handler, typename Operation, typename Allocator>
struct associated_allocator<
    asioext::detail::composed_op<Handler, Operation>, Allocator>
{
  typedef typename associated_allocator<Handler, Allocator>::type type;

  static type get(const asioext::detail::composed_op<Handler, Operation>& h,
                  const Allocator& a = Allocator()) ASIOEXT_NOEXCEPT
  {
    return associated_allocator<Handler, Allocator>::get(h.handler_, a);
  }
};

template <typename Handler, typename Operation, typename Executor>
struct associated_executor<
    asioext::detail::composed_op<Handler, Operation>, Executor>
{
  typedef typename associated_executor<Handler, Executor>::type type;

  static type get(const asioext::detail::composed_op<Handler, Operation>& h,
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
