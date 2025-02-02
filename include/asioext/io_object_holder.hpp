/// @file
/// Defines the io_object_holder helper class.
///
/// @copyright Copyright (c) 2024 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)
#ifndef ASIOEXT_IOOBJECTHOLDER_HPP
#define ASIOEXT_IOOBJECTHOLDER_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include "asioext/execution_context.hpp"

#if defined(ASIOEXT_USE_BOOST_ASIO)
# include <boost/asio/execution/executor.hpp>
# include <boost/asio/execution/context.hpp>
# include <boost/asio/any_io_executor.hpp>
# include <boost/asio/query.hpp>
#else
# include <asio/execution/executor.hpp>
# include <asio/execution/context.hpp>
# include <asio/any_io_executor.hpp>
# include <asio/query.hpp>
#endif

ASIOEXT_NS_BEGIN

template <typename IoObjectService, typename Executor>
class io_object_holder
{
public:
  /// The type of the service that will be used to provide I/O operations.
  typedef IoObjectService service_type;

  /// The underlying implementation type of I/O object.
  typedef typename service_type::implementation_type implementation_type;

  /// The type of the executor associated with the object.
  typedef Executor executor_type;

  // Construct an I/O object using an executor.
  explicit io_object_holder(const executor_type& ex)
      noexcept(noexcept(std::declval<IoObjectService>().construct(std::declval<implementation_type&>())))
    : service_(&asio::use_service<IoObjectService>(
          io_object_holder::get_context(ex)))
    , executor_(ex)
  {
    service_->construct(implementation_);
  }

  // Construct an I/O object using an execution context.
  template <execution_context ExecutionContext>
  explicit io_object_holder(ExecutionContext& context)
      noexcept(noexcept(std::declval<IoObjectService>().construct(std::declval<implementation_type&>())))
    : service_(&asio::use_service<IoObjectService>(context))
    , executor_(context.get_executor())
  {
    service_->construct(implementation_);
  }

  // Move-construct an I/O object.
  io_object_holder(io_object_holder&& other)
      noexcept(noexcept(std::declval<IoObjectService>().move_construct(std::declval<implementation_type&>(),
                                                                       std::declval<implementation_type&>())))
    : service_(&other.get_service())
    , executor_(other.get_executor())
  {
    service_->move_construct(implementation_, other.implementation_);
  }

  // Perform converting move-construction of an I/O object on the same service.
  template <typename Executor1>
  io_object_holder(io_object_holder<IoObjectService, Executor1>&& other)
      noexcept(noexcept(std::declval<IoObjectService>().move_construct(std::declval<implementation_type&>(),
                                                                       std::declval<implementation_type&>())))
    : service_(&other.get_service())
    , executor_(other.get_executor())
  {
    service_->move_construct(implementation_, other.get_implementation());
  }

  // Perform converting move-construction of an I/O object on another service.
  template <typename IoObjectService1, typename Executor1>
  io_object_holder(io_object_holder<IoObjectService1, Executor1>&& other)
      noexcept(noexcept(std::declval<IoObjectService>().converting_move_construct(
        std::declval<implementation_type&>(), std::declval<IoObjectService1&>(),
        std::declval<typename IoObjectService1::implementation_type&>())))
    : service_(&asio::use_service<IoObjectService>(
            io_object_holder::get_context(other.get_executor())))
    , executor_(other.get_executor())
  {
    service_->converting_move_construct(implementation_,
        other.get_service(), other.get_implementation());
  }

  // Destructor.
  ~io_object_holder()
  {
    service_->destroy(implementation_);
  }

  // Move-assign an I/O object.
  io_object_holder& operator=(io_object_holder&& other)
      noexcept(noexcept(std::declval<IoObjectService>().move_assign(std::declval<implementation_type&>(),
                                                                    std::declval<implementation_type&>())))
  {
    if (this != &other)
    {
      service_->move_assign(implementation_,
          *other.service_, other.implementation_);
      executor_.~executor_type();
      new (&executor_) executor_type(other.executor_);
      service_ = other.service_;
    }
    return *this;
  }

  // Disallow copying and copy assignment.
  io_object_holder(const io_object_holder&) = delete;
  io_object_holder& operator=(const io_object_holder&) = delete;

  // Get the executor associated with the object.
  const executor_type& get_executor() noexcept
  {
    return executor_;
  }

  // Get the service associated with the I/O object.
  service_type& get_service()
  {
    return *service_;
  }

  // Get the service associated with the I/O object.
  const service_type& get_service() const
  {
    return *service_;
  }

  // Get the underlying implementation of the I/O object.
  implementation_type& get_implementation()
  {
    return implementation_;
  }

  // Get the underlying implementation of the I/O object.
  const implementation_type& get_implementation() const
  {
    return implementation_;
  }

private:
  // Helper function to get an executor's context.
  template <typename T>
  static asio::execution_context& get_context(const T& t,
      std::enable_if_t<asio::execution::is_executor<T>::value>* = 0)
  {
    return asio::query(t, asio::execution::context);
  }

  // Helper function to get an executor's context.
  template <typename T>
  static asio::execution_context& get_context(const T& t,
      std::enable_if_t<!asio::execution::is_executor<T>::value>* = 0)
  {
    return t.context();
  }

  // The service associated with the I/O object.
  service_type* service_;

  // The underlying implementation of the I/O object.
  implementation_type implementation_;

  // The associated executor.
  executor_type executor_;
};

ASIOEXT_NS_END

#endif
