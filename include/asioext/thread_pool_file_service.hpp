/// @file
/// Declares the thread_pool_file_service class.
///
/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_THREADPOOLFILESERVICE_HPP
#define ASIOEXT_THREADPOOLFILESERVICE_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
#pragma once
#endif

#include "asioext/file_handle.hpp"
#include "asioext/open_args.hpp"
#include "asioext/file_perms.hpp"
#include "asioext/file_attrs.hpp"
#include "asioext/seek_origin.hpp"
#include "asioext/cancellation_token.hpp"
#include "asioext/async_result.hpp"

#include "asioext/detail/cstdint.hpp"
#include "asioext/detail/service_base.hpp"
#include "asioext/detail/mutex.hpp"

#if defined(ASIOEXT_USE_BOOST_ASIO)
# include <boost/asio/thread_pool.hpp>
# include <boost/asio/io_context.hpp>
#else
# include <asio/thread_pool.hpp>
# include <asio/io_context.hpp>
#endif

#if defined(ASIOEXT_HAS_BOOST_FILESYSTEM) || defined(ASIOEXT_IS_DOCUMENTATION)
# include <boost/filesystem/path.hpp>
#endif

ASIOEXT_NS_BEGIN

/// @ingroup files_handle
/// @brief A FileService utilizing a thread-pool for async operations.
///
/// This FileService class uses a thread-pool to emulate asynchronous file I/O.
class thread_pool_file_service
#if !defined(ASIOEXT_IS_DOCUMENTATION)
  : public asioext::detail::io_context_service_base<thread_pool_file_service>
#else
  : public asio::io_context::service
#endif
{
public:
#if defined(ASIOEXT_IS_DOCUMENTATION)
  /// The unique service identifier.
  static asio::io_context::id id;
#endif

#if defined(ASIOEXT_IS_DOCUMENTATION)
  /// The native handle type.
  typedef implementation_defined native_handle_type;
#else
  typedef file_handle::native_handle_type native_handle_type;
#endif

#if defined(ASIOEXT_IS_DOCUMENTATION)
  /// The type of a file implementation.
  typedef implementation_defined implementation_type;
#else
  class implementation_type
  {
  public:
    implementation_type()
      : next_(0)
      , prev_(0)
    {
      // ctor
    }

  private:
    // Only this service will have access to the internal values.
    friend class thread_pool_file_service;

    file_handle handle_;
    cancellation_token_source cancel_token_;

    // Pointers to adjacent handle implementations in linked list.
    implementation_type* next_;
    implementation_type* prev_;
  };
#endif

  /// Construct a new file service for the specified io_context.
  ///
  /// @param owner The io_context that owns this service object.
  ///
  /// @param num_threads The number of threads that shall be spawned to
  /// execute file I/O operations. Defaults to 1.
  ASIOEXT_DECL explicit thread_pool_file_service(asio::io_context& owner,
                                                 std::size_t num_threads = 1);

  /// Destroy all user-defined handler objects owned by the service.
  ASIOEXT_DECL void shutdown_service();

  /// Construct a new file implementation.
  ASIOEXT_DECL void construct(implementation_type& impl);

#ifdef ASIOEXT_HAS_MOVE
  /// Move-construct a new file implementation.
  ASIOEXT_DECL void move_construct(implementation_type& impl,
                                   implementation_type& other_impl)
    ASIOEXT_NOEXCEPT;

  /// Move-assign from another file implementation.
  ASIOEXT_DECL void move_assign(implementation_type& impl,
                                thread_pool_file_service& other_service,
                                implementation_type& other_impl);
#endif

  /// Destroy a file implementation.
  ASIOEXT_DECL void destroy(implementation_type& impl);

  /// Open a handle to the given file.
  ASIOEXT_DECL void open(implementation_type& impl,
                         const char* filename,
                         const open_args& args,
                         error_code& ec) ASIOEXT_NOEXCEPT;

#if defined(ASIOEXT_WINDOWS) || defined(ASIOEXT_IS_DOCUMENTATION)
  /// Open a handle to the given file.
  ASIOEXT_DECL void open(implementation_type& impl,
                         const wchar_t* filename,
                         const open_args& args,
                         error_code& ec) ASIOEXT_NOEXCEPT;
#endif

#if defined(ASIOEXT_HAS_BOOST_FILESYSTEM) || defined(ASIOEXT_IS_DOCUMENTATION)
  /// Open a handle to the given file.
  ASIOEXT_DECL void open(implementation_type& impl,
                         const boost::filesystem::path& filename,
                         const open_args& args,
                         error_code& ec) ASIOEXT_NOEXCEPT;
#endif

  /// Assign a native handle to a file implementation.
  ASIOEXT_DECL void assign(implementation_type& impl,
                           const native_handle_type& handle,
                           error_code& ec) ASIOEXT_NOEXCEPT;

  /// Determine whether the file handle is open.
  bool is_open(const implementation_type& impl) const ASIOEXT_NOEXCEPT
  {
    return impl.handle_.is_open();
  }

  /// Destroy a file implementation.
  ASIOEXT_DECL void close(implementation_type& impl, error_code& ec)
    ASIOEXT_NOEXCEPT;

  /// Get the native file handle representation.
  native_handle_type native_handle(implementation_type& impl) ASIOEXT_NOEXCEPT
  {
    return impl.handle_.native_handle();
  }

  /// Get the current file pointer position.
  ASIOEXT_DECL uint64_t position(implementation_type& impl,
                                 error_code& ec) ASIOEXT_NOEXCEPT;

  /// Change the current file pointer position.
  ASIOEXT_DECL uint64_t seek(implementation_type& impl,
                             seek_origin origin,
                             int64_t offset,
                             error_code& ec) ASIOEXT_NOEXCEPT;

  /// Get the file size.
  ASIOEXT_DECL uint64_t size(implementation_type& impl,
                             error_code& ec) ASIOEXT_NOEXCEPT;

  /// Set the file size.
  ASIOEXT_DECL void truncate(implementation_type& impl, uint64_t new_size,
                             error_code& ec) ASIOEXT_NOEXCEPT;

  /// Get the file permissions.
  ASIOEXT_DECL file_perms permissions(implementation_type& impl,
                                      error_code& ec) ASIOEXT_NOEXCEPT;

  /// Set the file permissions.
  ASIOEXT_DECL void permissions(implementation_type& impl,
                                file_perms new_perms,
                                error_code& ec) ASIOEXT_NOEXCEPT;

  /// Set the file permissions.
  ASIOEXT_DECL void permissions(implementation_type& impl,
                                file_perms new_perms, file_perm_options opts,
                                error_code& ec) ASIOEXT_NOEXCEPT;

  /// Get the file attributes.
  ASIOEXT_DECL file_attrs attributes(implementation_type& impl,
                                     error_code& ec) ASIOEXT_NOEXCEPT;

  /// Set the file attributes.
  ASIOEXT_DECL void attributes(implementation_type& impl,
                               file_attrs new_attrs,
                               error_code& ec) ASIOEXT_NOEXCEPT;

  /// Set the file attributes.
  ASIOEXT_DECL void attributes(implementation_type& impl,
                               file_attrs new_attrs, file_attr_options opts,
                               error_code& ec) ASIOEXT_NOEXCEPT;

  /// Get the file times.
  ASIOEXT_DECL file_times times(implementation_type& impl,
                                error_code& ec) ASIOEXT_NOEXCEPT;

  /// Set the file times.
  ASIOEXT_DECL void times(implementation_type& impl,
                          const file_times& new_times,
                          error_code& ec) ASIOEXT_NOEXCEPT;

  /// Cancel all operations associated with the handle.
  ASIOEXT_DECL void cancel(implementation_type& impl,
                           error_code& ec) ASIOEXT_NOEXCEPT;

  /// Read some data. Returns the number of bytes received.
  template <typename MutableBufferSequence>
  size_t read_some(implementation_type& impl,
                   const MutableBufferSequence& buffers,
                   error_code& ec) ASIOEXT_NOEXCEPT;

  /// Write the given data. Returns the number of bytes written.
  template <typename ConstBufferSequence>
  size_t write_some(implementation_type& impl,
                    const ConstBufferSequence& buffers,
                    error_code& ec) ASIOEXT_NOEXCEPT;

  /// Read some data at a specified offset. Returns the number of bytes received.
  template <typename MutableBufferSequence>
  size_t read_some_at(implementation_type& impl, uint64_t offset,
                      const MutableBufferSequence& buffers,
                      error_code& ec) ASIOEXT_NOEXCEPT;

  /// Write the given data at the specified offset. Returns the number of bytes
  /// written.
  template <typename ConstBufferSequence>
  size_t write_some_at(implementation_type& impl, uint64_t offset,
                       const ConstBufferSequence& buffers,
                       error_code& ec) ASIOEXT_NOEXCEPT;

  /// Start an asynchronous read. The buffer for the data being received must be
  /// valid for the lifetime of the asynchronous operation.
  template <typename MutableBufferSequence, typename Handler>
  ASIOEXT_INITFN_RESULT_TYPE(Handler, void(error_code, std::size_t))
  async_read_some(implementation_type& impl,
                  const MutableBufferSequence& buffers,
                  Handler&& handler);

  /// Start an asynchronous write. The data being written must be valid for the
  /// lifetime of the asynchronous operation.
  template <typename ConstBufferSequence, typename Handler>
  ASIOEXT_INITFN_RESULT_TYPE(Handler, void(error_code, std::size_t))
  async_write_some(implementation_type& impl,
                   const ConstBufferSequence& buffers,
                   Handler&& handler);

  /// Start an asynchronous read at a specified offset. The buffer for the data
  /// being received must be valid for the lifetime of the asynchronous
  /// operation.
  template <typename MutableBufferSequence, typename Handler>
  ASIOEXT_INITFN_RESULT_TYPE(Handler, void(error_code, std::size_t))
  async_read_some_at(implementation_type& impl, uint64_t offset,
                     const MutableBufferSequence& buffers,
                     Handler&& handler);

  /// Start an asynchronous write at a specified offset. The data being written
  /// must be valid for the lifetime of the asynchronous operation.
  template <typename ConstBufferSequence, typename Handler>
  ASIOEXT_INITFN_RESULT_TYPE(Handler, void(error_code, std::size_t))
  async_write_some_at(implementation_type& impl, uint64_t offset,
                      const ConstBufferSequence& buffers,
                      Handler&& handler);

  /// @private
  // This is needed for tests.
  asio::thread_pool& get_thread_pool()
  {
    return pool_;
  }

private:
  // Helper function to close a handle when the associated object is being
  // destroyed.
  ASIOEXT_DECL void close_for_destruction(implementation_type& impl);

  // The thread pool.
  asio::thread_pool pool_;

  // Mutex to protect access to the linked list of implementations.
  detail::mutex mutex_;

  // The head of a linked list of all implementations.
  implementation_type* impl_list_;
};

ASIOEXT_NS_END

#include "asioext/impl/thread_pool_file_service.hpp"

#if defined(ASIOEXT_HEADER_ONLY)
# include "asioext/impl/thread_pool_file_service.cpp"
#endif

#endif
