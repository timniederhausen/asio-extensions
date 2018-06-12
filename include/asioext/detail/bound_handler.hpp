/// @copyright Copyright (c) 2018 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_DETAIL_BOUNDHANDLER_HPP
#define ASIOEXT_DETAIL_BOUNDHANDLER_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

// gets us the macros we use below
#include "asioext/composed_operation.hpp"

#include <tuple>
#include <utility>

ASIOEXT_NS_BEGIN

namespace detail {

template <typename Handler, typename Tuple, std::size_t... Is>
void invoke(Handler& handler, const Tuple& t, std::index_sequence<Is...>)
  ASIOEXT_NOEXCEPT_IF(noexcept(handler(std::get<Is>(t)...)))
{
  handler(std::get<Is>(t)...);
}

template <typename Handler, typename... Args>
class bound_handler
{
#if !defined(ASIOEXT_IS_DOCUMENTATION) && (ASIOEXT_ASIO_VERSION >= 101100)
  template <typename T, typename Executor>
  friend struct asio::associated_allocator;

  template <typename T, typename Allocator>
  friend struct asio::associated_executor;
#endif

  friend void* asio_handler_allocate(std::size_t size,
                                     bound_handler* this_handler)
  {
    return ASIOEXT_HANDLER_ALLOC_HELPERS_NS::allocate(
        size, this_handler->handler_);
  }

  friend void asio_handler_deallocate(void* pointer, std::size_t size,
                                      bound_handler* this_handler)
  {
    ASIOEXT_HANDLER_ALLOC_HELPERS_NS::deallocate(
        pointer, size, this_handler->handler_);
  }

  friend bool asio_handler_is_continuation(bound_handler* this_handler)
  {
    return ASIOEXT_HANDLER_CONT_HELPERS_NS::is_continuation(
        this_handler->handler_);
  }

  template <typename Function>
  friend void asio_handler_invoke(Function& function,
                                  bound_handler* this_handler)
  {
    ASIOEXT_HANDLER_INVOKE_HELPERS_NS::invoke(
        function, this_handler->handler_);
  }

  template <typename Function>
  friend void asio_handler_invoke(const Function& function,
                                  bound_handler* this_handler)
  {
    ASIOEXT_HANDLER_INVOKE_HELPERS_NS::invoke(
        function, this_handler->handler_);
  }

public:
  template <typename Handler2, typename... Args2>
  explicit bound_handler(Handler2&& handler, Args2&&... args)
    : handler_(std::forward<Handler2>(handler))
    , args_(std::forward<Args2>(args)...)
  {
    // ctor
  }

  void operator()()
    ASIOEXT_NOEXCEPT_IF(noexcept(asioext::detail::invoke(
          handler_, args_, args_indices_type{})))
  {
    asioext::detail::invoke(handler_, args_, args_indices_type{});
  }

private:
  typedef std::index_sequence_for<Args...> args_indices_type;

  Handler handler_;
  std::tuple<Args...> args_;
};

}

ASIOEXT_NS_END

#if !defined(ASIOEXT_IS_DOCUMENTATION) && (ASIOEXT_ASIO_VERSION >= 101100)
# if defined(ASIOEXT_USE_BOOST_ASIO)
namespace boost {
# endif
namespace asio {

template <typename Handler, typename... Args, typename Allocator>
struct associated_allocator<
    asioext::detail::bound_handler<Handler, Args...>, Allocator>
{
  typedef typename associated_allocator<Handler, Allocator>::type type;

  static type get(const asioext::detail::bound_handler<Handler, Args...>& h,
                  const Allocator& a = Allocator()) ASIOEXT_NOEXCEPT
  {
    return associated_allocator<Handler, Allocator>::get(h.handler_, a);
  }
};

template <typename Handler, typename... Args, typename Executor>
struct associated_executor<
    asioext::detail::bound_handler<Handler, Args...>, Executor>
{
  typedef typename associated_executor<Handler, Executor>::type type;

  static type get(const asioext::detail::bound_handler<Handler, Args...>& h,
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
