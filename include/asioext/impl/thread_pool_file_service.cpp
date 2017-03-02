/// @copyright Copyright (c) 2017 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#include "asioext/thread_pool_file_service.hpp"
#include "asioext/open.hpp"

#include "asioext/detail/error.hpp"

#ifdef ASIOEXT_HAS_MOVE
# include <utility>
#endif

ASIOEXT_NS_BEGIN

void thread_pool_file_service::thread_function::operator()()
{
  error_code ec;
  service_->run(ec);
}

thread_pool_file_service::thread_pool_file_service(
    asio::io_service& io_service, std::size_t num_threads)
  : service_base(io_service)
  , work_(pool_)
  , impl_list_(0)
{
  work_.on_work_started();

  thread_function f = { &pool_ };
  pool_threads_.create_threads(f, num_threads);
}

void thread_pool_file_service::shutdown_service()
{
  // Close all implementations, causing all operations to complete.
  detail::mutex::scoped_lock lock(mutex_);
  for (implementation_type* cur = impl_list_; cur; cur = cur->next_)
    close_for_destruction(*cur);

  work_.on_work_finished();
}

void thread_pool_file_service::construct(implementation_type& impl)
{
  impl.cancel_token_.reset();

  // Insert implementation into linked list of all implementations.
  detail::mutex::scoped_lock lock(mutex_);
  impl.next_ = impl_list_;
  impl.prev_ = 0;
  if (impl_list_)
    impl_list_->prev_ = &impl;
  impl_list_ = &impl;
}

#ifdef ASIOEXT_HAS_MOVE
void thread_pool_file_service::move_construct(implementation_type& impl,
                                              implementation_type& other_impl)
{
  impl.handle_ = other_impl.handle_;
  other_impl.handle_.clear();

  impl.cancel_token_ = std::move(other_impl.cancel_token_);

  // Insert implementation into linked list of all implementations.
  detail::mutex::scoped_lock lock(mutex_);
  impl.next_ = impl_list_;
  impl.prev_ = 0;
  if (impl_list_)
    impl_list_->prev_ = &impl;
  impl_list_ = &impl;
}

void thread_pool_file_service::move_assign(
    implementation_type& impl, thread_pool_file_service& other_service,
    implementation_type& other_impl)
{
  close_for_destruction(impl);

  if (this != &other_service) {
    // Remove implementation from linked list of all implementations.
    detail::mutex::scoped_lock lock(mutex_);
    if (impl_list_ == &impl)
      impl_list_ = impl.next_;
    if (impl.prev_)
      impl.prev_->next_ = impl.next_;
    if (impl.next_)
      impl.next_->prev_= impl.prev_;
    impl.next_ = 0;
    impl.prev_ = 0;
  }

  impl.handle_ = other_impl.handle_;
  other_impl.handle_.clear();

  if (this != &other_service) {
    // Insert implementation into linked list of all implementations.
    detail::mutex::scoped_lock lock(other_service.mutex_);
    impl.next_ = other_service.impl_list_;
    impl.prev_ = 0;
    if (other_service.impl_list_)
      other_service.impl_list_->prev_ = &impl;
    other_service.impl_list_ = &impl;
  }
}
#endif

void thread_pool_file_service::destroy(implementation_type& impl)
{
  close_for_destruction(impl);

  // Remove implementation from linked list of all implementations.
  detail::mutex::scoped_lock lock(mutex_);
  if (impl_list_ == &impl)
    impl_list_ = impl.next_;
  if (impl.prev_)
    impl.prev_->next_ = impl.next_;
  if (impl.next_)
    impl.next_->prev_= impl.prev_;
  impl.next_ = 0;
  impl.prev_ = 0;
}

void thread_pool_file_service::open(implementation_type& impl,
                                    const char* filename,
                                    open_flags flags,
                                    error_code& ec)
{
  if (impl.handle_.is_open()) {
    ec = asio::error::already_open;
    return;
  }
  impl.handle_ = asioext::open(filename, flags, ec);
}

#if defined(ASIOEXT_WINDOWS) || defined(ASIOEXT_IS_DOCUMENTATION)
void thread_pool_file_service::open(implementation_type& impl,
                                    const wchar_t* filename,
                                    open_flags flags,
                                    error_code& ec)
{
  if (impl.handle_.is_open()) {
    ec = asio::error::already_open;
    return;
  }
  impl.handle_ = asioext::open(filename, flags, ec);
}
#endif

#if defined(ASIOEXT_HAS_BOOST_FILESYSTEM) || defined(ASIOEXT_IS_DOCUMENTATION)
void thread_pool_file_service::open(implementation_type& impl,
                                    const boost::filesystem::path& filename,
                                    open_flags flags,
                                    error_code& ec)
{
  if (impl.handle_.is_open()) {
    ec = asio::error::already_open;
    return;
  }
  impl.handle_ = asioext::open(filename, flags, ec);
}
#endif

void thread_pool_file_service::assign(implementation_type& impl,
                                      const native_handle_type& handle,
                                      error_code& ec)
{
  if (impl.handle_.is_open()) {
    ec = asio::error::already_open;
    return;
  }

  impl.handle_ = handle;
  ec = error_code();
}

void thread_pool_file_service::close(implementation_type& impl, error_code& ec)
{
  impl.handle_.close(ec);
}

uint64_t thread_pool_file_service::size(implementation_type& impl,
                                        error_code& ec) ASIOEXT_NOEXCEPT
{
  return impl.handle_.size(ec);
}

uint64_t thread_pool_file_service::position(implementation_type& impl,
                                            error_code& ec) ASIOEXT_NOEXCEPT
{
  return impl.handle_.position(ec);
}

uint64_t thread_pool_file_service::seek(implementation_type& impl,
                                        seek_origin origin,
                                        int64_t offset,
                                        error_code& ec) ASIOEXT_NOEXCEPT
{
  return impl.handle_.seek(origin, offset, ec);
}

void thread_pool_file_service::cancel(implementation_type& impl,
                                      error_code& ec)
{
  if (!impl.handle_.is_open()) {
    ec = asio::error::bad_descriptor;
    return;
  }

  impl.cancel_token_.cancel();
  // TODO(tim): log handler operation
}

void thread_pool_file_service::close_for_destruction(implementation_type& impl)
{
  if (impl.handle_.is_open()) {
    // TODO(tim): log handler operation
    impl.cancel_token_.destroy();
    impl.handle_.close();
  }
}

ASIOEXT_NS_END
