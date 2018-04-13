/// @file
/// Declares the asio_hook_allocator class and the asioext::hook_allocator
/// trait.
///
/// @copyright Copyright (c) 2016 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_HOOKALLOCATOR_HPP
#define ASIOEXT_HOOKALLOCATOR_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include "asioext/detail/asio_version.hpp"

#if defined(ASIOEXT_USE_BOOST_ASIO)
# include <boost/asio/detail/handler_alloc_helpers.hpp>
#else
# include <asio/detail/handler_alloc_helpers.hpp>
#endif

#if (ASIOEXT_ASIO_VERSION >= 101100)
# if defined(ASIOEXT_USE_BOOST_ASIO)
#  include <boost/asio/associated_allocator.hpp>
# else
#  include <asio/associated_allocator.hpp>
# endif
#endif

#if defined(ASIOEXT_USE_BOOST_ASIO)
# define ASIOEXT_HANDLER_ALLOC_HELPERS_NS boost_asio_handler_alloc_helpers
#else
# define ASIOEXT_HANDLER_ALLOC_HELPERS_NS asio_handler_alloc_helpers
#endif

ASIOEXT_NS_BEGIN

template <typename T, typename Handler>
class asio_hook_allocator;

namespace detail {

template <typename Handler, typename Allocator>
struct hook_allocator_aux
{
  typedef Allocator type;

  static type get(Handler&, const Allocator& a) ASIOEXT_NOEXCEPT
  {
    return a;
  }
};

template <typename Handler, typename T>
struct hook_allocator_aux<Handler, std::allocator<T> >
{
  typedef asio_hook_allocator<T, Handler> type;

  static type get(Handler& handler, const std::allocator<T>&) ASIOEXT_NOEXCEPT
  {
    return type(handler);
  }
};

}

/// @ingroup core
/// @brief Allocator that uses a handler's memory allocation hooks.
///
/// Asio @c Handlers (before Asio 1.11.0+) use two hooks to allow memory
/// allocation to be customized:
///
/// * asio_handler_allocate()
/// * asio_handler_deallocate()
///
/// This stateless allocator uses these hooks to allocate/free memory.
/// @note The allocated memory's lifetime is dependent on the handler's
/// lifetime. All memory must be de-allocated before the user's handler
/// is invoked ([async.reqmts.async.alloc]).
template <typename T, typename Handler>
class asio_hook_allocator
{
  template <class U, typename Handler2>
  friend class asio_hook_allocator;

  template <class U>
  friend bool operator==(const asio_hook_allocator&,
                         const asio_hook_allocator<U, Handler>&)
      ASIOEXT_NOEXCEPT;

  template <class U>
  friend bool operator!=(const asio_hook_allocator&,
                         const asio_hook_allocator<U, Handler>&)
      ASIOEXT_NOEXCEPT;

public:
  typedef T value_type;

  explicit asio_hook_allocator(Handler& h) ASIOEXT_NOEXCEPT
    : handler_(h)
  {
  }

  template <typename U>
  asio_hook_allocator(
      const asio_hook_allocator<U, Handler>& a) ASIOEXT_NOEXCEPT
    : handler_(a.handler_)
  {
  }

  T* allocate(std::size_t n)
  {
    return static_cast<T*>(
        ASIOEXT_HANDLER_ALLOC_HELPERS_NS::allocate(sizeof(T) * n, handler_));
  }

  void deallocate(T* p, std::size_t n)
  {
    ASIOEXT_HANDLER_ALLOC_HELPERS_NS::deallocate(p, sizeof(T) * n, handler_);
  }

private:
  Handler& handler_;
};

#if !defined(ASIOEXT_IS_DOCUMENTATION)
template <typename Handler>
class asio_hook_allocator<void, Handler>
{
  template <class U, typename Handler2>
  friend class asio_hook_allocator;

  template <class U>
  friend bool operator==(const asio_hook_allocator&,
                         const asio_hook_allocator<U, Handler>&)
      ASIOEXT_NOEXCEPT;

  template <class U>
  friend bool operator!=(const asio_hook_allocator&,
                         const asio_hook_allocator<U, Handler>&)
      ASIOEXT_NOEXCEPT;

public:
  typedef void value_type;

  explicit asio_hook_allocator(Handler& h) ASIOEXT_NOEXCEPT
    : handler_(h)
  {
  }

  template <typename U>
  asio_hook_allocator(const asio_hook_allocator<U, Handler>& a) ASIOEXT_NOEXCEPT
    : handler_(a.handler_)
  {
  }

private:
  Handler& handler_;
};
#endif

template <typename Handler, class T, class U>
bool operator==(const asio_hook_allocator<T, Handler>& a,
                const asio_hook_allocator<U, Handler>& b) ASIOEXT_NOEXCEPT
{ return std::addressof(a.handler_) == std::addressof(b.handler_); }

template <typename Handler, class T, class U>
bool operator!=(const asio_hook_allocator<T, Handler>& a,
                const asio_hook_allocator<U, Handler>& b) ASIOEXT_NOEXCEPT
{ return std::addressof(a.handler_) != std::addressof(b.handler_); }

/// @ingroup core
/// @brief Helper trait to obtain the allocator associated with a handler.
///
/// Retrieves an @c Allocator that is used to allocate memory tied to the
/// handler's asynchronous operation.
///
/// Supports the @c associated_allocator trait introduced by
/// the Networking TS / Asio 1.11.0+, as well as Asio's memory allocation
/// hooks (which this trait will wrap in a @c asio_hook_allocator)
///
/// @note This trait is useful for libraries / applications that need to
/// deal with both types of allocation customizations.
template <typename Handler>
class hook_allocator
{
#if (ASIOEXT_ASIO_VERSION >= 101100)
  typedef typename asio::associated_allocator<Handler>::type
      associated_allocator_type;
#else
  typedef std::allocator<void> associated_allocator_type;
#endif

  typedef detail::hook_allocator_aux<Handler,
      associated_allocator_type> helper_type;

public:
  /// @brief The Handler's ProtoAllocator.
  ///
  /// A type meeting ProtoAllocator requirements that if rebound
  /// can be used to allocate memory using the handler's allocator.
  typedef typename helper_type::type type;

  /// @brief Get a ProtoAllocator for the given handler.
  static type get(Handler& handler) ASIOEXT_NOEXCEPT
  {
#if (ASIOEXT_ASIO_VERSION >= 101100)
    return helper_type::get(handler, asio::get_associated_allocator(handler));
#else
    return helper_type::get(handler, associated_allocator_type());
#endif
  }
};

/// @ingroup core
/// @brief Get the handler's associated allocator.
///
/// Calling this function is equivalent to calling
/// <code>hook_allocator<Handler>::get(handler)</code>.
/// See @c hook_allocator for details.
template <typename Handler>
typename hook_allocator<Handler>::type get_hook_allocator(Handler& handler)
    ASIOEXT_NOEXCEPT
{
  return hook_allocator<Handler>::get(handler);
}

#if defined(ASIOEXT_HAS_ALIAS_TEMPLATES)
/// @ingroup core
/// @brief Alias for hook_allocator::type
template <typename Handler>
using hook_allocator_t = typename hook_allocator<Handler>::type;
#endif

ASIOEXT_NS_END

#endif
