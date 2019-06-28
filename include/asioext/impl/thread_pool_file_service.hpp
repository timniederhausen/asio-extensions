/// @copyright Copyright (c) 2017 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_IMPL_THREADPOOLFILESERVICE_HPP
#define ASIOEXT_IMPL_THREADPOOLFILESERVICE_HPP

#include "asioext/file_handle.hpp"
#include "asioext/compose.hpp"
#include "asioext/error_code.hpp"

#include "asioext/detail/error.hpp"

#if defined(ASIOEXT_USE_BOOST_ASIO)
# include <boost/asio/bind_executor.hpp>
# include <boost/asio/post.hpp>
#else
# include <asio/bind_executor.hpp>
# include <asio/post.hpp>
#endif

ASIOEXT_NS_BEGIN

namespace detail {

template <typename OperationImpl>
class thread_pool_fs_op
{
public:
  thread_pool_fs_op(const cancellation_token_source& source,
                    OperationImpl& impl,
                    thread_pool_file_service* svc)
    : cancel_token_(source)
    , impl_(std::move(impl))
    , svc_(svc)
  {
    // ctor
  }

  template <typename Self>
  void operator()(Self& self)
  {
    error_code ec;
    std::size_t bytes_transferred = 0;
    if (cancel_token_.cancelled()) {
      ec = asio::error::operation_aborted;
    } else {
      bytes_transferred = impl_(ec);
    }
    auto ex = get_associated_executor(self, svc_->get_io_context().get_executor());
    asio::post(ex, self.complete_to_bound_handler(ec, bytes_transferred));
  }

private:
  cancellation_token cancel_token_;
  OperationImpl impl_;
  thread_pool_file_service* svc_;
};

struct thread_pool_fs_init
{
  template <typename Handler, typename Implementation>
  void operator()(Handler&& handler,
                  Implementation&& impl,
                  const cancellation_token_source& source,
                  thread_pool_file_service* svc)
  {
    auto ex = get_associated_executor(handler, svc->get_io_context().get_executor());
    auto op = make_composed_operation(
        thread_pool_fs_op<Implementation>(source, impl, svc),
        std::move(handler), ex);

    asio::post(asio::bind_executor(svc->get_thread_pool(), std::move(op)));
  }
};

template <typename MutableBufferSequence>
struct thread_pool_fs_read
{
  std::size_t operator()(error_code& ec) ASIOEXT_NOEXCEPT
  {
    return handle.read_some(buffers, ec);
  }

  file_handle handle;
  MutableBufferSequence buffers;
};

template <typename ConstBufferSequence>
struct thread_pool_fs_write
{
  std::size_t operator()(error_code& ec) ASIOEXT_NOEXCEPT
  {
    return handle.write_some(buffers, ec);
  }

  file_handle handle;
  ConstBufferSequence buffers;
};

template <typename MutableBufferSequence>
struct thread_pool_fs_read_at
{
  std::size_t operator()(error_code& ec) ASIOEXT_NOEXCEPT
  {
    return handle.read_some_at(offset, buffers, ec);
  }

  file_handle handle;
  uint64_t offset;
  MutableBufferSequence buffers;
};

template <typename ConstBufferSequence>
struct thread_pool_fs_write_at
{
  std::size_t operator()(error_code& ec) ASIOEXT_NOEXCEPT
  {
    return handle.write_some_at(offset, buffers, ec);
  }

  file_handle handle;
  uint64_t offset;
  ConstBufferSequence buffers;
};

}

template <typename MutableBufferSequence>
size_t thread_pool_file_service::read_some(
    implementation_type& impl, const MutableBufferSequence& buffers,
    error_code& ec) ASIOEXT_NOEXCEPT
{
  return impl.handle_.read_some(buffers, ec);
}

template <typename ConstBufferSequence>
size_t thread_pool_file_service::write_some(implementation_type& impl,
                                            const ConstBufferSequence& buffers,
                                            error_code& ec) ASIOEXT_NOEXCEPT
{
  return impl.handle_.write_some(buffers, ec);
}

template <typename MutableBufferSequence>
size_t thread_pool_file_service::read_some_at(
    implementation_type& impl, uint64_t offset,
    const MutableBufferSequence& buffers, error_code& ec) ASIOEXT_NOEXCEPT
{
  return impl.handle_.read_some_at(offset, buffers, ec);
}

template <typename ConstBufferSequence>
size_t thread_pool_file_service::write_some_at(
    implementation_type& impl, uint64_t offset,
    const ConstBufferSequence& buffers, error_code& ec) ASIOEXT_NOEXCEPT
{
  return impl.handle_.write_some_at(offset, buffers, ec);
}

template <typename MutableBufferSequence, typename CompletionToken>
ASIOEXT_INITFN_RESULT_TYPE(CompletionToken, void(error_code, std::size_t))
thread_pool_file_service::async_read_some(implementation_type& impl,
                                          const MutableBufferSequence& buffers,
                                          CompletionToken&& token)
{
  return async_initiate<CompletionToken, void(error_code, std::size_t)>(
      detail::thread_pool_fs_init(), token,
      detail::thread_pool_fs_read<MutableBufferSequence>{impl.handle_, buffers},
      impl.cancel_token_, this);
}

template <typename ConstBufferSequence, typename CompletionToken>
ASIOEXT_INITFN_RESULT_TYPE(CompletionToken, void(error_code, std::size_t))
thread_pool_file_service::async_write_some(implementation_type& impl,
                                           const ConstBufferSequence& buffers,
                                           CompletionToken&& token)
{
  return async_initiate<CompletionToken, void(error_code, std::size_t)>(
      detail::thread_pool_fs_init(), token,
      detail::thread_pool_fs_write<ConstBufferSequence>{impl.handle_, buffers},
      impl.cancel_token_, this);
}

template <typename MutableBufferSequence, typename CompletionToken>
ASIOEXT_INITFN_RESULT_TYPE(CompletionToken, void(error_code, std::size_t))
thread_pool_file_service::async_read_some_at(
    implementation_type& impl, uint64_t offset,
    const MutableBufferSequence& buffers,
    CompletionToken&& token)
{
  return async_initiate<CompletionToken, void(error_code, std::size_t)>(
      detail::thread_pool_fs_init(), token,
      detail::thread_pool_fs_read_at<MutableBufferSequence>{impl.handle_, offset, buffers},
      impl.cancel_token_, this);
}

template <typename ConstBufferSequence, typename CompletionToken>
ASIOEXT_INITFN_RESULT_TYPE(CompletionToken, void(error_code, std::size_t))
thread_pool_file_service::async_write_some_at(
    implementation_type& impl, uint64_t offset,
    const ConstBufferSequence& buffers, CompletionToken&& token)
{
  return async_initiate<CompletionToken, void(error_code, std::size_t)>(
      detail::thread_pool_fs_init(), token,
      detail::thread_pool_fs_write_at<ConstBufferSequence>{impl.handle_, offset, buffers},
      impl.cancel_token_, this);
}

ASIOEXT_NS_END

#endif
