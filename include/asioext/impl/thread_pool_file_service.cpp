/// @copyright Copyright (c) 2017 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#include "asioext/thread_pool_file_service.hpp"
#include "asioext/open.hpp"

#include "asioext/detail/error.hpp"

ASIOEXT_NS_BEGIN

thread_pool_file_service::thread_pool_file_service(
    asio::io_context& owner, std::size_t num_threads)
  : io_context_service_base(owner)
  , pool_(num_threads)
  , impl_list_(0)
{
}

void thread_pool_file_service::shutdown_service()
{
  // Close all implementations, causing all operations to complete.
  detail::mutex::scoped_lock lock(mutex_);
  for (implementation_type* cur = impl_list_; cur; cur = cur->next_)
    close_for_destruction(*cur);

  pool_.stop();
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
void thread_pool_file_service::move_construct(
    implementation_type& impl, implementation_type& other_impl) ASIOEXT_NOEXCEPT
{
  impl.handle_ = other_impl.handle_;
  other_impl.handle_.clear();

  impl.cancel_token_ =
      ASIOEXT_MOVE_CAST(cancellation_token_source)(other_impl.cancel_token_);

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
                                    const open_args& args,
                                    error_code& ec) ASIOEXT_NOEXCEPT
{
  if (impl.handle_.is_open()) {
    ec = asio::error::already_open;
    return;
  }
  impl.handle_ = asioext::open(filename, args, ec).release();
}

#if defined(ASIOEXT_WINDOWS) || defined(ASIOEXT_IS_DOCUMENTATION)
void thread_pool_file_service::open(implementation_type& impl,
                                    const wchar_t* filename,
                                    const open_args& args,
                                    error_code& ec) ASIOEXT_NOEXCEPT
{
  if (impl.handle_.is_open()) {
    ec = asio::error::already_open;
    return;
  }
  impl.handle_ = asioext::open(filename, args, ec).release();
}
#endif

#if defined(ASIOEXT_HAS_BOOST_FILESYSTEM) || defined(ASIOEXT_IS_DOCUMENTATION)
void thread_pool_file_service::open(implementation_type& impl,
                                    const boost::filesystem::path& filename,
                                    const open_args& args,
                                    error_code& ec) ASIOEXT_NOEXCEPT
{
  if (impl.handle_.is_open()) {
    ec = asio::error::already_open;
    return;
  }
  impl.handle_ = asioext::open(filename, args, ec).release();
}
#endif

void thread_pool_file_service::assign(implementation_type& impl,
                                      const native_handle_type& handle,
                                      error_code& ec) ASIOEXT_NOEXCEPT
{
  if (impl.handle_.is_open()) {
    ec = asio::error::already_open;
    return;
  }

  impl.handle_ = handle;
  ec = error_code();
}

void thread_pool_file_service::close(implementation_type& impl,
                                     error_code& ec) ASIOEXT_NOEXCEPT
{
  impl.handle_.close(ec);
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

uint64_t thread_pool_file_service::size(implementation_type& impl,
                                        error_code& ec) ASIOEXT_NOEXCEPT
{
  return impl.handle_.size(ec);
}

void thread_pool_file_service::truncate(implementation_type& impl,
                                        uint64_t new_size,
                                        error_code& ec) ASIOEXT_NOEXCEPT
{
  impl.handle_.truncate(new_size, ec);
}

file_perms thread_pool_file_service::permissions(
    implementation_type& impl, error_code& ec) ASIOEXT_NOEXCEPT
{
  return impl.handle_.permissions(ec);
}

void thread_pool_file_service::permissions(
    implementation_type& impl, file_perms new_perms,
    error_code& ec) ASIOEXT_NOEXCEPT
{
  impl.handle_.permissions(new_perms, ec);
}

void thread_pool_file_service::permissions(
    implementation_type& impl, file_perms new_perms,
    file_perm_options opts, error_code& ec) ASIOEXT_NOEXCEPT
{
  impl.handle_.permissions(new_perms, opts, ec);
}

file_attrs thread_pool_file_service::attributes(
    implementation_type& impl, error_code& ec) ASIOEXT_NOEXCEPT
{
  return impl.handle_.attributes(ec);
}

void thread_pool_file_service::attributes(
    implementation_type& impl, file_attrs new_attrs,
    error_code& ec) ASIOEXT_NOEXCEPT
{
  impl.handle_.attributes(new_attrs, ec);
}

void thread_pool_file_service::attributes(
    implementation_type& impl, file_attrs new_attrs,
    file_attr_options opts, error_code& ec) ASIOEXT_NOEXCEPT
{
  impl.handle_.attributes(new_attrs, opts, ec);
}

file_times thread_pool_file_service::times(implementation_type& impl,
                                           error_code& ec) ASIOEXT_NOEXCEPT
{
  return impl.handle_.times(ec);
}

void thread_pool_file_service::times(implementation_type& impl,
                                     const file_times& new_times,
                                     error_code& ec) ASIOEXT_NOEXCEPT
{
  impl.handle_.times(new_times, ec);
}

void thread_pool_file_service::cancel(implementation_type& impl,
                                      error_code& ec) ASIOEXT_NOEXCEPT
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
