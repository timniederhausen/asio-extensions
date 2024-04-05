/// @file
/// Declares the asioext::composed_operation utility class.
///
/// @copyright Copyright (c) 2016 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_COMPOSE_HPP
#define ASIOEXT_COMPOSE_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include "asioext/async_result.hpp"
#include "asioext/bind_handler.hpp"
#include "asioext/work.hpp"
#include "asioext/detail/asio_version.hpp"
#include "asioext/detail/move_support.hpp"

#if defined(ASIOEXT_USE_BOOST_ASIO)
# include <boost/asio/detail/handler_cont_helpers.hpp>
# define ASIOEXT_HANDLER_CONT_HELPERS_NS boost_asio_handler_cont_helpers
#else
# include <asio/detail/handler_cont_helpers.hpp>
# define ASIOEXT_HANDLER_CONT_HELPERS_NS asio_handler_cont_helpers
#endif

#if (ASIOEXT_ASIO_VERSION < 101700)
# if defined(ASIOEXT_USE_BOOST_ASIO)
#  include <boost/asio/detail/handler_alloc_helpers.hpp>
#  include <boost/asio/detail/handler_cont_helpers.hpp>
#  include <boost/asio/detail/handler_invoke_helpers.hpp>
#  define ASIOEXT_HANDLER_ALLOC_HELPERS_NS boost_asio_handler_alloc_helpers
#  define ASIOEXT_HANDLER_INVOKE_HELPERS_NS boost_asio_handler_invoke_helpers
# else
#  include <asio/detail/handler_alloc_helpers.hpp>
#  include <asio/detail/handler_invoke_helpers.hpp>
#  define ASIOEXT_HANDLER_ALLOC_HELPERS_NS asio_handler_alloc_helpers
#  define ASIOEXT_HANDLER_INVOKE_HELPERS_NS asio_handler_invoke_helpers
# endif
#endif

#if (ASIOEXT_ASIO_VERSION >= 101100)
# if defined(ASIOEXT_USE_BOOST_ASIO)
#  include <boost/asio/associated_allocator.hpp>
#  include <boost/asio/associated_executor.hpp>
#  include <boost/asio/executor_work_guard.hpp>
# else
#  include <asio/associated_allocator.hpp>
#  include <asio/associated_executor.hpp>
#  include <asio/executor_work_guard.hpp>
# endif
#endif

#if (ASIOEXT_ASIO_VERSION >= 102400)
# if defined(ASIOEXT_USE_BOOST_ASIO)
#  include <boost/asio/compose.hpp>
# else
#  include <asio/compose.hpp>
# endif
#endif

ASIOEXT_NS_BEGIN

template <typename Handler, typename Implementation, typename Work>
class composed_operation;

namespace detail {

struct initiate_composed_op
{
  template <typename Handler, typename Impl, typename Work>
  void operator()(Handler&& handler, Impl&& impl, Work&& work) const
  {
    composed_operation<
      typename std::decay<Handler>::type,
      typename std::decay<Impl>::type,
      typename std::decay<Work>::type
    >(std::move(handler),
      std::move(impl),
      std::move(work))();
  }
};

template <typename Impl, typename Handler, typename Work>
composed_operation<
  typename std::decay<Handler>::type,
  typename std::decay<Impl>::type,
  typename std::decay<Work>::type
> make_composed_operation_aux(Impl&& impl, Handler&& handler, Work&& work)
{
  return composed_operation<
    typename std::decay<Handler>::type,
    typename std::decay<Impl>::type,
    typename std::decay<Work>::type
  >(std::move(handler),
    std::move(impl),
    std::move(work));
}

template <typename IoObject, typename = typename std::enable_if<
  !asio::is_executor<IoObject>::value
#if (ASIOEXT_ASIO_VERSION >= 102400)
  && !asio::execution::is_executor<IoObject>::value
#endif
>::type>
inline typename IoObject::executor_type
get_executor(IoObject& io_object)
{
  return io_object.get_executor();
}

template <typename Executor, typename = typename std::enable_if<
  asio::is_executor<Executor>::value
#if (ASIOEXT_ASIO_VERSION >= 102400)
  || asio::execution::is_executor<Executor>::value
#endif
>::type>
inline const Executor& get_executor(const Executor& ex)
{ return ex; }

}

/// @ingroup core
/// @brief Wrapper for a stateful composed operation.
///
/// This helper class simplifies stateful composed operations
/// that wrap a user-provided handler.
///
/// It acts as an intermediate completion handler
/// for a provided @c Implementation, which will be invoked whenever
/// this object is invoked. The first argument to @c implementation
/// is a reference to the @c composed_operation object itself, followed
/// by all arguments this wrapper has been invoked with.
///
/// Hooks and customization points are inherited from the wrapped
/// user-provided handler.
template <typename Handler, typename Implementation, typename Work>
class composed_operation
{
#if !defined(ASIOEXT_IS_DOCUMENTATION) && (ASIOEXT_ASIO_VERSION < 101700)
  friend void* asio_handler_allocate(std::size_t size,
                                     composed_operation* this_handler)
  {
    return ASIOEXT_HANDLER_ALLOC_HELPERS_NS::allocate(
        size, this_handler->handler_);
  }

  friend void asio_handler_deallocate(void* pointer, std::size_t size,
                                      composed_operation* this_handler)
  {
    ASIOEXT_HANDLER_ALLOC_HELPERS_NS::deallocate(
        pointer, size, this_handler->handler_);
  }

  template <typename Function>
  friend void asio_handler_invoke(Function& function,
                                  composed_operation* this_handler)
  {
    ASIOEXT_HANDLER_INVOKE_HELPERS_NS::invoke(
        function, this_handler->handler_);
  }

  template <typename Function>
  friend void asio_handler_invoke(const Function& function,
                                  composed_operation* this_handler)
  {
    ASIOEXT_HANDLER_INVOKE_HELPERS_NS::invoke(
        function, this_handler->handler_);
  }
#endif

#if !defined(ASIOEXT_IS_DOCUMENTATION)
  friend bool asio_handler_is_continuation(composed_operation* this_handler)
  {
    if (this_handler->state_ > 1)
      return true;
    return ASIOEXT_HANDLER_CONT_HELPERS_NS::is_continuation(
        this_handler->handler_);
  }
#endif

  Handler handler_;
  Implementation impl_;
  Work work_;
  unsigned int state_;

public:
  /// @brief Construct a new @c composed_operation.
  ///
  /// @param handler The user-defined handler to wrap.
  /// @param impl The operation's implementation.
  /// @param work @c asio::executor_work_guard objects
  /// that must be kept alive until after the operation's complete.
  template <typename HandlerArg, typename ImplementationArg, typename WorkArg>
  composed_operation(HandlerArg&& handler, ImplementationArg&& impl, WorkArg&& work)
    : handler_(std::forward<HandlerArg>(handler))
    , impl_(std::forward<ImplementationArg>(impl))
    , work_(std::forward<WorkArg>(work))
    , state_(0)
  {
    // ctor
  }

  /// @brief Invoke the operation's implementation.
  ///
  /// Invokes the @c implementation as if by
  /// @code
  /// implementation(*this, std::forward<Args>(args)...)
  /// @endcode
  template <typename... Args>
  void operator()(Args&&... args)
#if !defined(__GNUC__) || defined(__clang__)
  // https://travis-ci.org/timniederhausen/asio-extensions/jobs/586780540
  // GCC incorrectly rejects *this inside the noexcept clause
  // see: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=52869
    ASIOEXT_NOEXCEPT_IF(noexcept(impl_(*this, std::forward<Args>(args)...)))
#endif
  {
    if (state_ < 2)
      ++state_;
    impl_(*this, std::forward<Args>(args)...);
  }

  /// @brief Complete this composed operation.
  ///
  /// Completes the composed operation by invoking the user-provided
  /// handler as if by
  /// @code
  /// handler(std::forward<Args>(args)...)
  /// @endcode
  /// Afterwards, all held work is released.
  ///
  /// @param args Arguments for the user-provided handler.
  template <typename... Args>
  void complete(Args&&... args)
  {
    handler_(std::forward<Args>(args)...);
    work_.reset();
  }

  /// @brief Complete this composed operation.
  ///
  /// Completes the composed operation by creating a function object that
  /// upon invocation calls as the user-provided handler as if by
  /// @code
  /// handler(std::forward<Args>(args)...)
  /// @endcode
  /// After the handler has been invoked, all held work is released.
  ///
  /// @param args Arguments for the user-provided handler.
  template <typename... Args>
  auto complete_to_bound_handler(Args&&... args)
  {
    return bind_handler(std::move(handler_), std::move(work_),
                        std::forward<Args>(args)...);
  }

  /// @name Getters for the wrapped handler
  ///
  /// These are quite dangerous, you should almost never have to use them.
  ///
  /// @{

  Handler& get_handler() noexcept { return handler_; }
  const Handler& get_handler() const noexcept { return handler_; }

  /// @}
};

#if defined(ASIOEXT_IS_DOCUMENTATION)
/// @ingroup core
/// @brief Construct a @c composed_operation
template <typename CompletionToken, typename Signature,
          typename Implementation, typename... IoObjectsOrExecutors>
implementation_defined make_composed_operation(
    Implementation&& implementation, CompletionToken& token,
    IoObjectsOrExecutors&&... io_objects_or_executors);

/// @ingroup core
/// @brief Launch an asynchronous operation with a stateful implementation.
///
/// This helper function constructs a @c composed_operation from the
/// given arguments and uses @c async_initiate to launch it.
template <typename CompletionToken, typename Signature,
          typename Implementation, typename... IoObjectsOrExecutors>
implementation_defined async_compose(
    Implementation&& implementation, CompletionToken& token,
    IoObjectsOrExecutors&&... io_objects_or_executors);
#else
template <typename Handler, typename Implementation, typename... IoObjectsOrExecutors>
auto make_composed_operation(Implementation&& implementation, Handler&& handler,
                             IoObjectsOrExecutors&&... io_objects_or_executors)
{
  return detail::make_composed_operation_aux(
      std::forward<Implementation>(implementation),
      std::forward<Handler>(handler),
      make_work_tuple(
        detail::get_executor(
            std::forward<IoObjectsOrExecutors>(io_objects_or_executors))...
      ));
}

# if (ASIOEXT_ASIO_VERSION >= 102400)
using asio::async_compose;
# else
template <typename CompletionToken, typename Signature,
          typename Implementation, typename... IoObjectsOrExecutors>
ASIOEXT_INITFN_RESULT_TYPE(CompletionToken, Signature)
async_compose(Implementation&& implementation, CompletionToken& token,
              IoObjectsOrExecutors&&... io_objects_or_executors)
{
  return async_initiate<CompletionToken, Signature>(
      detail::initiate_composed_op(), token,
      std::forward<Implementation>(implementation),
      make_work_tuple(
        detail::get_executor(
            std::forward<IoObjectsOrExecutors>(io_objects_or_executors))...
      )
  );
}
# endif
#endif

ASIOEXT_NS_END

#if !defined(ASIOEXT_IS_DOCUMENTATION) && (ASIOEXT_ASIO_VERSION >= 101100)
# if defined(ASIOEXT_USE_BOOST_ASIO)
namespace boost {
# endif
namespace asio {

template <typename Handler, typename Implementation, typename Work, typename Allocator>
struct associated_allocator<
    asioext::composed_operation<Handler, Implementation, Work>, Allocator>
{
  typedef typename associated_allocator<Handler, Allocator>::type type;

  static type get(const asioext::composed_operation<Handler, Implementation, Work>& h,
                  const Allocator& a = Allocator()) ASIOEXT_NOEXCEPT
  {
    return associated_allocator<Handler, Allocator>::get(h.get_handler(), a);
  }
};

template <typename Handler, typename Implementation, typename Work, typename Executor>
struct associated_executor<
    asioext::composed_operation<Handler, Implementation, Work>, Executor>
{
  typedef typename associated_executor<Handler, Executor>::type type;

  static type get(const asioext::composed_operation<Handler, Implementation, Work>& h,
                  const Executor& ex = Executor()) ASIOEXT_NOEXCEPT
  {
    return associated_executor<Handler, Executor>::get(h.get_handler(), ex);
  }
};

}
# if defined(ASIOEXT_USE_BOOST_ASIO)
}
# endif
#endif

#endif
