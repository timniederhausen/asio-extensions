/// @file
/// Defines the asioext::make_work_tuple utility function.
///
/// @copyright Copyright (c) 2019 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_WORK_HPP
#define ASIOEXT_WORK_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include "asioext/detail/asio_version.hpp"

#if (ASIOEXT_ASIO_VERSION >= 101100)
# if defined(ASIOEXT_USE_BOOST_ASIO)
#  include <boost/asio/executor_work_guard.hpp>
# else
#  include <asio/executor_work_guard.hpp>
# endif
#endif

ASIOEXT_NS_BEGIN

/// @brief Tuple of heterogenous @c asio::executor_work_guard objects.
///
/// This helper class represents a tuple of heterogenous
/// @c asio::executor_work_guard objects that act like one
/// @c asio::executor_work_guard object, i.e.
/// any action (`reset()`, destruction, ...) taken on an object of this
/// type is taken on all contained @c asio::executor_work_guard objects.
///
/// @note Use @ref make_work_tuple() to create objects of this type.
template <typename... Executors>
class work_tuple;

#if !defined(ASIOEXT_IS_DOCUMENTATION)
template <typename Executor>
class work_tuple<Executor>
{
public:
  work_tuple(const Executor& ex)
    : work_(ex)
  {}

  void reset()
  {
    work_.reset();
  }

private:
  asio::executor_work_guard<Executor> work_;
};

template <>
class work_tuple<void>
{
public:
  void reset()
  {
    // no-op
  }
};

template <typename Head, typename... Tail>
class work_tuple<Head, Tail...>
{
public:
  work_tuple(const Head& head, const Tail&... tail)
    : head_(head)
    , tail_(tail...)
  {}

  void reset()
  {
    head_.reset();
    tail_.reset();
  }

private:
  asio::executor_work_guard<Head> head_;
  work_tuple<Tail...> tail_;
};
#endif

#if defined(ASIOEXT_IS_DOCUMENTATION)
/// @ingroup core
/// @brief Wrap multiple @c asio::executor_work_guard in a single object.
///
/// This helper method creates @c asio::executor_work_guard objects for the
/// given @c Executor objects and wraps them in a single object which is
/// returned to the caller.
/// Any action (`reset()`, destruction, ...) taken on the returned object is
/// taken on all contained @c asio::executor_work_guard objects.
///
/// @param executors One or more objects conforming to the @c Executor
/// requirements.
/// @return A @c asio::executor_work_guard-like object wrapping work guards
/// for all given executors.
template <typename... Executors>
implementation_defined make_work_tuple(Executors&&... executors);
#else
inline work_tuple<void> make_work_tuple()
{
  return work_tuple<void>();
}

template <typename Head, typename... Tail>
inline work_tuple<Head, Tail...>
make_work_tuple(const Head& head, const Tail&... tail)
{
  return work_tuple<Head, Tail...>(head, tail...);
}
#endif

ASIOEXT_NS_END

#endif
