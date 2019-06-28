/// @copyright Copyright (c) 2018 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_BINDHANDLER_HPP
#define ASIOEXT_BINDHANDLER_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include "asioext/work.hpp"

#if !defined(ASIOEXT_HAS_VARIADIC_TEMPLATES)
# error "Your compiler doesn't support variadic templates (C++11)"
#endif
#if !defined(ASIOEXT_HAS_STD_INDEX_SEQUENCE)
# error "Your compiler doesn't support std::index_sequence (C++14)"
#endif

#include "asioext/detail/bound_handler.hpp"

#include <utility>

ASIOEXT_NS_BEGIN

#if defined(ASIOEXT_IS_DOCUMENTATION)
/// @ingroup core
/// @brief Bind values to a Handler's arguments to create a CompletionHandler.
///
/// This helper function binds the given @c args to the @c Handler,
/// creating a nullary function object that satisfies the requirements of
/// a CompletionHandler.
///
/// The original handler's hooks and customization points are retained and
/// will be used for the returned CompletionHandler as well.
///
/// @param handler The @c Handler object you wish to bind values to.
/// @param args The arguments the handler should be invoked with.
/// The values will be copied/moved and stored inside the returned function
/// object.
/// @returns A nullary function object that when called will invoke
/// the original handler with the bound arguments.
template <typename Handler, typename... Args>
implementation_defined bind_handler(Handler&& handler, Args&&... args);

/// @ingroup core
/// @brief Bind values to a Handler's arguments to create a CompletionHandler.
///
/// This helper function binds the given @c args to the @c Handler,
/// creating a nullary function object that satisfies the requirements of
/// a @c CompletionHandler. Additionally, the given tuple of
/// @c asio::executor_work_guard is kept alive until after @c handler is invoked.
///
/// The original handler's hooks and customization points are retained and
/// will be used for the returned CompletionHandler as well.
///
/// @param handler The @c Handler object you wish to bind values to.
/// @param work Additional @c asio::executor_work_guard objects that must
/// exist until after @c handler is invoked.
/// @param args The arguments the handler should be invoked with.
/// The values will be copied/moved and stored inside the returned function
/// object.
/// @returns A nullary function object that when called will invoke
/// the original handler with the bound arguments.
template <typename Handler, typename... Work, typename... Args>
implementation_defined bind_handler(Handler&& handler,
                                    work_tuple<Work...>&& work,
                                    Args&&... args);
#else
template <typename Handler, typename... Args>
detail::bound_handler<
    typename std::decay<Handler>::type,
    work_tuple<void>,
    typename std::decay<Args>::type...
> bind_handler(Handler&& handler, Args&&... args)
{
  return detail::bound_handler<
    typename std::decay<Handler>::type,
    work_tuple<void>,
    typename std::decay<Args>::type...
  >(std::forward<Handler>(handler), work_tuple<void>(),
    std::forward<Args>(args)...);
}

template <typename Handler, typename... Work, typename... Args>
detail::bound_handler<
    typename std::decay<Handler>::type,
    work_tuple<Work...>,
    typename std::decay<Args>::type...
> bind_handler(Handler&& handler, work_tuple<Work...>&& work, Args&&... args)
{
  return detail::bound_handler<
    typename std::decay<Handler>::type,
    work_tuple<Work...>,
    typename std::decay<Args>::type...
  >(std::forward<Handler>(handler), std::move(work),
    std::forward<Args>(args)...);
}
#endif

ASIOEXT_NS_END

#endif
