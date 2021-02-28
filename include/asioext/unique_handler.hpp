/// @file
/// Defines the unique_handler class
///
/// @copyright Copyright (c) 2019 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_UNIQUEHANDLER_HPP
#define ASIOEXT_UNIQUEHANDLER_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include "asioext/associated_allocator.hpp"

ASIOEXT_NS_BEGIN

namespace detail {

template <typename T, typename Handler>
T* allocate_operation(Handler& handler)
{
  using allocator_type = typename std::allocator_traits<
    asioext::associated_allocator_t<Handler>>::template rebind_alloc<T>;

  allocator_type a(asioext::get_associated_allocator(handler));
  return a.allocate(1);
}

template <typename T, typename Handler>
void deallocate_operation(T* ptr, Handler& handler)
{
  using allocator_type = typename std::allocator_traits<
    asioext::associated_allocator_t<Handler>>::template rebind_alloc<T>;

  allocator_type a(asioext::get_associated_allocator(handler));
  a.deallocate(ptr, 1);
}

ASIOEXT_DECL void throw_bad_function_call();

}

template <typename Signature>
class unique_handler;

/// @ingroup core
/// @brief Polymorphic wrapper for @c Handler functions.
///
/// This class is a type-erased wrapper for @c Handler compatible functions.
/// It can be seen as a @c std::function for @c Handler objects.
/// Instances of @c unique_handler can only be invoked once, which completes
/// the stored handler and resets the instance.
/// A @c unique_handler without a stored handler is called <em>empty</em>.
/// Invoking an empty @c unique_handler causes a @c asioext::bad_function_call exception.
///
/// @note This class should be preferred to @c std::function, as it uses
/// the handler's associated allocator and ensures the various @c Handler requirements are met.
template <typename... Args>
class unique_handler<void (Args...)>
{
public:
  /// @brief Construct an empty @c unique_handler.
  ASIOEXT_CONSTEXPR unique_handler() ASIOEXT_NOEXCEPT
    : storage_(nullptr)
  {}

  /// @brief Construct a @c unique_handler with the given @c Handler object.
  ///
  /// This constructor stores the given handler to this unique_handler
  /// object. The new handler will be moved into a specially allocated storage.
  ///
  /// @param handler The handler to be stored to this object.
  /// It is moved-constructed into @c unique_handler-owned storage.
  template <typename Handler>
  unique_handler(Handler&& handler)
    : storage_(new (detail::allocate_operation<storage_impl<std::decay_t<Handler>>,
                                                            std::decay_t<Handler>>(handler))
        storage_impl<std::decay_t<Handler>>(handler))
  {}

  /// @brief Destroy a @c unique_handler.
  ///
  /// This destructor destroys any stored handler without invoking it.
  ~unique_handler()
  {
    if (storage_)
      storage_->abort(storage_);
  }

  /// @brief Move-construct a unique_handler from another.
  ///
  /// This constructor moves a handle from one object to another.
  ///
  /// @param other The other unique_handler object from which the move will
  /// occur.
  ///
  /// @note Following the move, the moved-from object is in the same state as if
  /// constructed using the @c unique_handler() constructor.
  unique_handler(unique_handler&& other) ASIOEXT_NOEXCEPT
    : storage_(other.storage_)
  {
    other.storage_ = nullptr;
  }

  /// @brief Move-assign a unique_handler from another.
  ///
  /// This assignment operator moves a handle from one object to another.
  /// If this object already owns a handler, the current handler will be
  /// destroyed without invocation.
  ///
  /// @param other The other unique_handler object from which the move will
  /// occur.
  ///
  /// @note Following the move, the moved-from object is in the same state as if
  /// constructed using the @c unique_handler() constructor.
  unique_handler& operator=(unique_handler&& other) ASIOEXT_NOEXCEPT
  {
    if (storage_)
      storage_->abort(storage_);

    storage_ = other.storage_;
    other.storage_ = nullptr;
    return *this;
  }

  /// @brief Assign a new @c Handler object.
  ///
  /// This assignment operator binds a new handler to this unique_handler
  /// object. If this object already owns a handler, the current handler
  /// will be destroyed without invocation.
  /// The new handler will be moved into a specially allocated storage.
  ///
  /// @param handler The handler to be stored in this object.
  /// It is moved-constructed into @c unique_handler-owned storage.
  template <typename Handler>
  unique_handler& operator=(Handler&& handler)
  {
    if (storage_)
      storage_->abort(storage_);

    storage_ = new (detail::allocate_operation<storage_impl<std::decay_t<Handler>>,
                                               Handler>(handler))
        storage_impl<std::decay_t<Handler>>(std::move(handler));
    return *this;
  }

  /// @brief Complete the stored handler.
  ///
  /// This function invokes the stored handler with the given @c args.
  /// After this operation the handler is considered <em>completed</em>,
  /// meaning it cannot be invoked again.
  /// The @c unique_handler object is reset to an empty state.
  void complete(Args... args)
  {
    // Reset storage_ before invoking user-code - we might get a new handler.
    auto storage = storage_;
    if (storage) {
      storage_ = nullptr;
      storage->complete(storage, std::forward<Args>(args)...);
    } else {
      detail::throw_bad_function_call();
    }
  }

  /// @brief Reset the @c unique_handler object.
  ///
  /// This function resets the @c unique_handler object to an empty state.
  void reset()
  {
    if (storage_) {
      auto storage = storage_;
      storage_ = nullptr;
      storage->abort(storage);
    }
  }

  /// @brief Check whether this object stores a handler.
  explicit operator bool() const ASIOEXT_NOEXCEPT
  { return !!storage_; }

private:
  struct storage_base
  {
    void (* abort)(storage_base* self);
    void (* complete)(storage_base* self, Args&&... args);
  };

  template <typename Handler>
  struct storage_impl : storage_base
  {
    storage_impl(Handler& handler)
      : handler_(std::move(handler))
    {
      this->abort = do_abort;
      this->complete = do_complete;
    }

    static void do_abort(storage_base* self)
    {
      storage_impl* self_impl = static_cast<storage_impl*>(self);
      Handler handler(std::move(self_impl->handler_));
      self_impl->~storage_impl();
      detail::deallocate_operation(self_impl, handler);
    }

    static void do_complete(storage_base* self, Args&&... args)
    {
      storage_impl* self_impl = static_cast<storage_impl*>(self);
      Handler handler(std::move(self_impl->handler_));
      self_impl->~storage_impl();
      detail::deallocate_operation(self_impl, handler);
      handler(std::forward<Args>(args)...);
    }

    Handler handler_;
  };

  storage_base* storage_;
};

ASIOEXT_NS_END

#if defined(ASIOEXT_HEADER_ONLY)
# include "asioext/impl/unique_handler.cpp"
#endif

#endif
