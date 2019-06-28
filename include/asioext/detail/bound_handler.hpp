/// @copyright Copyright (c) 2018 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_DETAIL_BOUNDHANDLER_HPP
#define ASIOEXT_DETAIL_BOUNDHANDLER_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include "asioext/detail/asio_version.hpp"

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

template <typename Handler, typename Work, typename... Args>
class bound_handler : private Work
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

  typedef std::index_sequence_for<Args...> args_indices_type;

  Handler handler_;
  std::tuple<Args...> args_;

public:
  template <typename RawHandler, typename RawWork, typename... RawArgs>
  explicit bound_handler(RawHandler&& handler, RawWork&& work,
                         RawArgs&&... args)
    : Work(std::forward<RawWork>(work))
    , handler_(std::forward<RawHandler>(handler))
    , args_(std::forward<RawArgs>(args)...)
  {
    // ctor
  }

  void operator()()
    ASIOEXT_NOEXCEPT_IF(noexcept(asioext::detail::invoke(
          handler_, args_, args_indices_type{})))
  {
    this->reset(); // defined in Work
    asioext::detail::invoke(handler_, args_, args_indices_type{});
  }
};

}

ASIOEXT_NS_END

#if !defined(ASIOEXT_IS_DOCUMENTATION) && (ASIOEXT_ASIO_VERSION >= 101100)
# if defined(ASIOEXT_USE_BOOST_ASIO)
namespace boost {
# endif
namespace asio {

template <typename Handler, typename Work, typename... Args, typename Allocator>
struct associated_allocator<
    asioext::detail::bound_handler<Handler, Work, Args...>, Allocator>
{
  typedef typename associated_allocator<Handler, Allocator>::type type;

  static type get(const asioext::detail::bound_handler<Handler, Work, Args...>& h,
                  const Allocator& a = Allocator()) ASIOEXT_NOEXCEPT
  {
    return associated_allocator<Handler, Allocator>::get(h.handler_, a);
  }
};

template <typename Handler, typename Work, typename... Args, typename Executor>
struct associated_executor<
    asioext::detail::bound_handler<Handler, Work, Args...>, Executor>
{
  typedef typename associated_executor<Handler, Executor>::type type;

  static type get(const asioext::detail::bound_handler<Handler, Work, Args...>& h,
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
