/// @copyright Copyright (c) 2017 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_IMPL_THREADPOOLFILESERVICE_HPP
#define ASIOEXT_IMPL_THREADPOOLFILESERVICE_HPP

#include "asioext/file_handle.hpp"
#include "asioext/composed_operation.hpp"

#include "asioext/detail/error_code.hpp"
#include "asioext/detail/error.hpp"
#include "asioext/detail/move_support.hpp"
#include "asioext/detail/operation.hpp"

#if defined(ASIOEXT_USE_BOOST_ASIO)
# include <boost/asio/detail/bind_handler.hpp>
#else
# include <asio/detail/bind_handler.hpp>
#endif

ASIOEXT_NS_BEGIN

namespace detail {

template <typename MutableBufferSequence, typename Handler>
class read_some_op : public operation<Handler>
{
public:
  read_some_op(const cancellation_token_source& source, file_handle handle,
               const MutableBufferSequence& buffers,
               ASIOEXT_MOVE_ARG(Handler) handler,
               asio::io_service& io_service)
    : operation<Handler>(ASIOEXT_MOVE_CAST(Handler)(handler), io_service)
    , handle_(handle)
    , cancel_token_(source)
    , buffers_(buffers)
  {
    // ctor
  }

  void operator()();

private:
  file_handle handle_;
  cancellation_token cancel_token_;
  MutableBufferSequence buffers_;
};

template <typename ConstBufferSequence, typename Handler>
class write_some_op : public operation<Handler>
{
public:
  write_some_op(const cancellation_token_source& source, file_handle handle,
                const ConstBufferSequence& buffers,
                ASIOEXT_MOVE_ARG(Handler) handler,
                asio::io_service& io_service)
    : operation<Handler>(ASIOEXT_MOVE_CAST(Handler)(handler), io_service)
    , handle_(handle)
    , cancel_token_(source)
    , buffers_(buffers)
  {
    // ctor
  }

  void operator()();

private:
  file_handle handle_;
  cancellation_token cancel_token_;
  ConstBufferSequence buffers_;
};

template <typename MutableBufferSequence, typename Handler>
class read_some_at_op : public operation<Handler>
{
public:
  read_some_at_op(const cancellation_token_source& source, file_handle handle,
                  uint64_t offset, const MutableBufferSequence& buffers,
                  ASIOEXT_MOVE_ARG(Handler) handler,
                  asio::io_service& io_service)
    : operation<Handler>(ASIOEXT_MOVE_CAST(Handler)(handler), io_service)
    , handle_(handle)
    , cancel_token_(source)
    , offset_(offset)
    , buffers_(buffers)
  {
    // ctor
  }

  void operator()();

private:
  file_handle handle_;
  cancellation_token cancel_token_;
  uint64_t offset_;
  MutableBufferSequence buffers_;
};

template <typename ConstBufferSequence, typename Handler>
class write_some_at_op : public operation<Handler>
{
public:
  write_some_at_op(const cancellation_token_source& source, file_handle handle,
                   uint64_t offset, const ConstBufferSequence& buffers,
                   ASIOEXT_MOVE_ARG(Handler) handler,
                   asio::io_service& io_service)
    : operation<Handler>(ASIOEXT_MOVE_CAST(Handler)(handler), io_service)
    , handle_(handle)
    , cancel_token_(source)
    , offset_(offset)
    , buffers_(buffers)
  {
    // ctor
  }

  void operator()();

private:
  file_handle handle_;
  cancellation_token cancel_token_;
  uint64_t offset_;
  ConstBufferSequence buffers_;
};

template <typename MutableBufferSequence, typename Handler>
void read_some_op<MutableBufferSequence, Handler>::operator()()
{
  error_code ec;
  std::size_t bytes_transferred = 0;
  if (cancel_token_.cancelled()) {
    ec = asio::error::operation_aborted;
  } else {
    bytes_transferred = handle_.read_some(buffers_, ec);
  }
  this->get_executor().dispatch(asio::detail::bind_handler(
      ASIOEXT_MOVE_CAST(Handler)(this->handler_), ec, bytes_transferred));
}

template <typename ConstBufferSequence, typename Handler>
void write_some_op<ConstBufferSequence, Handler>::operator()()
{
  error_code ec;
  std::size_t bytes_transferred = 0;
  if (cancel_token_.cancelled()) {
    ec = asio::error::operation_aborted;
  } else {
    bytes_transferred = handle_.write_some(buffers_, ec);
  }
  this->get_executor().dispatch(asio::detail::bind_handler(
      ASIOEXT_MOVE_CAST(Handler)(this->handler_), ec, bytes_transferred));
}

template <typename MutableBufferSequence, typename Handler>
void read_some_at_op<MutableBufferSequence, Handler>::operator()()
{
  error_code ec;
  std::size_t bytes_transferred = 0;
  if (cancel_token_.cancelled()) {
    ec = asio::error::operation_aborted;
  } else {
    bytes_transferred = handle_.read_some_at(offset_, buffers_, ec);
  }
  this->get_executor().dispatch(asio::detail::bind_handler(
      ASIOEXT_MOVE_CAST(Handler)(this->handler_), ec, bytes_transferred));
}

template <typename ConstBufferSequence, typename Handler>
void write_some_at_op<ConstBufferSequence, Handler>::operator()()
{
  error_code ec;
  std::size_t bytes_transferred = 0;
  if (cancel_token_.cancelled()) {
    ec = asio::error::operation_aborted;
  } else {
    bytes_transferred = handle_.write_some_at(offset_, buffers_, ec);
  }
  this->get_executor().dispatch(asio::detail::bind_handler(
      ASIOEXT_MOVE_CAST(Handler)(this->handler_), ec, bytes_transferred));
}

}

template <typename MutableBufferSequence>
size_t thread_pool_file_service::read_some(
    implementation_type& impl, const MutableBufferSequence& buffers,
    error_code& ec)
{
  return impl.handle_.read_some(buffers, ec);
}

template <typename ConstBufferSequence>
size_t thread_pool_file_service::write_some(implementation_type& impl,
                                            const ConstBufferSequence& buffers,
                                            error_code& ec)
{
  return impl.handle_.write_some(buffers, ec);
}

template <typename MutableBufferSequence>
size_t thread_pool_file_service::read_some_at(
    implementation_type& impl, uint64_t offset,
    const MutableBufferSequence& buffers, error_code& ec)
{
  return impl.handle_.read_some_at(offset, buffers, ec);
}

template <typename ConstBufferSequence>
size_t thread_pool_file_service::write_some_at(
    implementation_type& impl, uint64_t offset,
    const ConstBufferSequence& buffers, error_code& ec)
{
  return impl.handle_.write_some_at(offset, buffers, ec);
}

template <typename MutableBufferSequence, typename Handler>
ASIOEXT_INITFN_RESULT_TYPE(Handler, void(error_code, std::size_t))
thread_pool_file_service::async_read_some(implementation_type& impl,
                                          const MutableBufferSequence& buffers,
                                          ASIOEXT_MOVE_ARG(Handler) handler)
{
  typedef detail::read_some_op<MutableBufferSequence,
      ASIOEXT_HANDLER_TYPE(Handler, void (error_code, std::size_t))
  > operation;

  async_completion<Handler, void (error_code, std::size_t)> init(handler);
  operation op(impl.cancel_token_, impl.handle_, buffers,
               ASIOEXT_MOVE_CAST(Handler)(init.completion_handler),
               this->get_io_service());
  pool_.post(ASIOEXT_MOVE_CAST(operation)(op));
  return init.result.get();
}

template <typename ConstBufferSequence, typename Handler>
ASIOEXT_INITFN_RESULT_TYPE(Handler, void(error_code, std::size_t))
thread_pool_file_service::async_write_some(implementation_type& impl,
                                           const ConstBufferSequence& buffers,
                                           ASIOEXT_MOVE_ARG(Handler) handler)
{
  typedef detail::write_some_op<ConstBufferSequence,
      ASIOEXT_HANDLER_TYPE(Handler, void (error_code, std::size_t))
  > operation;

  async_completion<Handler, void (error_code, std::size_t)> init(handler);
  operation op(impl.cancel_token_, impl.handle_, buffers,
               ASIOEXT_MOVE_CAST(Handler)(init.completion_handler),
               this->get_io_service());
  pool_.post(ASIOEXT_MOVE_CAST(operation)(op));
  return init.result.get();
}

template <typename MutableBufferSequence, typename Handler>
ASIOEXT_INITFN_RESULT_TYPE(Handler, void(error_code, std::size_t))
thread_pool_file_service::async_read_some_at(
    implementation_type& impl, uint64_t offset,
    const MutableBufferSequence& buffers,
    ASIOEXT_MOVE_ARG(Handler) handler)
{
  typedef detail::read_some_at_op<MutableBufferSequence,
      ASIOEXT_HANDLER_TYPE(Handler, void (error_code, std::size_t))
  > operation;

  async_completion<Handler, void (error_code, std::size_t)> init(handler);
  operation op(impl.cancel_token_, impl.handle_, buffers,
               ASIOEXT_MOVE_CAST(Handler)(init.completion_handler),
               this->get_io_service());
  pool_.post(ASIOEXT_MOVE_CAST(operation)(op));
  return init.result.get();
}

template <typename ConstBufferSequence, typename Handler>
ASIOEXT_INITFN_RESULT_TYPE(Handler, void(error_code, std::size_t))
thread_pool_file_service::async_write_some_at(
    implementation_type& impl, uint64_t offset,
    const ConstBufferSequence& buffers, ASIOEXT_MOVE_ARG(Handler) handler)
{
  typedef detail::write_some_at_op<ConstBufferSequence,
      ASIOEXT_HANDLER_TYPE(Handler, void (error_code, std::size_t))
  > operation;

  async_completion<Handler, void (error_code, std::size_t)> init(handler);
  operation op(impl.cancel_token_, impl.handle_, buffers,
               ASIOEXT_MOVE_CAST(Handler)(init.completion_handler),
               this->get_io_service());
  pool_.post(ASIOEXT_MOVE_CAST(operation)(op));
  return init.result.get();
}

ASIOEXT_NS_END

#endif
