/// @file
/// Defines the basic_file class
///
/// @copyright Copyright (c) 2016 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_BASICFILE_HPP
#define ASIOEXT_BASICFILE_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include "asioext/file_handle.hpp"
#include "asioext/open_args.hpp"
#include "asioext/file_perms.hpp"
#include "asioext/file_attrs.hpp"
#include "asioext/seek_origin.hpp"
#include "asioext/error_code.hpp"
#include "asioext/io_object_holder.hpp"
#include "asioext/async_result.hpp"

#include "asioext/detail/throw_error.hpp"

#if defined(ASIOEXT_HAS_BOOST_FILESYSTEM) || defined(ASIOEXT_IS_DOCUMENTATION)
# include <boost/filesystem/path.hpp>
#endif

#include <utility>

ASIOEXT_NS_BEGIN

/// @ingroup files_handle
/// @brief Basic interface for (a)synchronous file I/O.
///
/// This class template provides a generic interface for (a)synchronous
/// file I/O.
///
/// basic_file models the following asio concepts:
/// * SyncRandomAccessReadDevice
/// * SyncRandomAccessWriteDevice
/// * SyncReadStream
/// * SyncWriteStream
/// * AsyncReadStream
/// * AsyncWriteStream
/// * AsyncRandomAccessReadDevice
/// * AsyncRandomAccessWriteDevice
///
/// @par Thread Safety:
/// @e Distinct @e objects: Safe.@n
/// @e Shared @e objects: Unsafe.
template <typename FileService, typename Executor = asio::any_io_executor>
class basic_file
{
  // All files have access to each other's implementations.
  template <typename FileService1, typename Executor1>
  friend class basic_file;

public:
  /// The type of the executor associated with the object.
  typedef Executor executor_type;

  /// Rebinds the file type to another executor.
  template <typename Executor1>
  struct rebind_executor
  {
    /// The file type when rebound to the specified executor.
    typedef basic_file<FileService, Executor1> other;
  };

  /// The operating system's native file handle type.
  typedef typename FileService::native_handle_type native_handle_type;

  /// A basic_file is always the lowest layer.
  typedef basic_file lowest_layer_type;

  /// @brief Construct an unopened file.
  ///
  /// @param ex The I/O executor that the file will use, by default, to
  /// dispatch handlers for any asynchronous operations performed on the file.
  explicit basic_file(const executor_type& ex)
    : holder_(ex)
  {
    // ctor
  }

  /// @brief Construct an unopened file.
  ///
  /// @param context An execution context which provides the I/O executor that
  /// the file will use, by default, to dispatch handlers for any asynchronous
  /// operations performed on the file.
  template <execution_context ExecutionContext>
  explicit basic_file(ExecutionContext& context)
    : holder_(context)
  {
    // ctor
  }

  /// @brief Construct a file using a native handle object.
  ///
  /// This constructor takes ownership of the given wrapped native handle.
  ///
  /// @param ex The I/O executor that the file will use, by default, to
  /// dispatch handlers for any asynchronous operations performed on the file.
  ///
  /// @param handle The native handle object, wrapped in a file_handle,
  /// which shall be assigned to this basic_file object.
  basic_file(const executor_type& ex, const file_handle& handle)
    : holder_(ex)
  {
    error_code ec;
    holder_.get_service().assign(holder_.get_implementation(), handle.native_handle(), ec);
    detail::throw_error(ec, "basic_file construct");
  }

  /// @brief Construct a file using a native handle object.
  ///
  /// This constructor takes ownership of the given wrapped native handle.
  ///
  /// @param context An execution context which provides the I/O executor that
  /// the file will use, by default, to dispatch handlers for any asynchronous
  /// operations performed on the file.
  ///
  /// @param handle The native handle object, wrapped in a file_handle,
  /// which shall be assigned to this basic_file object.
  template <execution_context ExecutionContext>
  basic_file(ExecutionContext& context, const file_handle& handle)
    : holder_(context)
  {
    error_code ec;
    holder_.get_service().assign(holder_.get_implementation(), handle.native_handle(), ec);
    detail::throw_error(ec, "basic_file construct");
  }

  /// @brief Construct a file using a native handle object.
  ///
  /// This constructor takes ownership of the given native handle.
  ///
  /// @param ex The I/O executor that the file will use, by default, to
  /// dispatch handlers for any asynchronous operations performed on the file.
  ///
  /// @param handle The native handle object which shall be assigned to
  /// this basic_file object.
  basic_file(const executor_type& ex, const native_handle_type& handle)
    : holder_(ex)
  {
    error_code ec;
    holder_.get_service().assign(holder_.get_implementation(), handle.native_handle(), ec);
    detail::throw_error(ec, "basic_file construct");
  }

  /// @brief Construct a file using a native handle object.
  ///
  /// This constructor takes ownership of the given native handle.
  ///
  /// @param context An execution context which provides the I/O executor that
  /// the file will use, by default, to dispatch handlers for any asynchronous
  /// operations performed on the file.
  ///
  /// @param handle The native handle object which shall be assigned to
  /// this basic_file object.
  template <execution_context ExecutionContext>
  basic_file(ExecutionContext& context, const native_handle_type& handle)
    : holder_(context)
  {
    error_code ec;
    holder_.get_service().assign(holder_.get_implementation(), handle.native_handle(), ec);
    detail::throw_error(ec, "basic_file construct");
  }

  /// @brief Open a file and construct a basic_file.
  ///
  /// This constructor opens a new handle to the given file.
  ///
  /// For details, see @ref open(const char*,const open_args&)
  ///
  /// @param ex The I/O executor that the file will use, by default, to
  /// dispatch handlers for any asynchronous operations performed on the file.
  ///
  /// @param filename The path of the file to open.
  ///
  /// @param args Additional options used to open the file.
  ///
  /// @throws asio::system_error Thrown on failure.
  ///
  /// @see open_flags
  basic_file(const executor_type& ex, const char* filename, const open_args& args)
    : holder_(ex)
  {
    error_code ec;
    holder_.get_service().open(holder_.get_implementation(), filename, args, ec);
    detail::throw_error(ec, "basic_file construct");
  }

  /// @brief Open a file and construct a basic_file.
  ///
  /// This constructor opens a new handle to the given file.
  ///
  /// For details, see @ref open(const char*,const open_args&)
  ///
  /// @param context An execution context which provides the I/O executor that
  /// the file will use, by default, to dispatch handlers for any asynchronous
  /// operations performed on the file.
  ///
  /// @param filename The path of the file to open.
  ///
  /// @param args Additional options used to open the file.
  ///
  /// @throws asio::system_error Thrown on failure.
  ///
  /// @see open_flags
  template <execution_context ExecutionContext>
  basic_file(ExecutionContext& context, const char* filename, const open_args& args)
    : holder_(context)
  {
    error_code ec;
    holder_.get_service().open(holder_.get_implementation(), filename, args, ec);
    detail::throw_error(ec, "basic_file construct");
  }

  /// @brief Open a file and construct a basic_file.
  ///
  /// This constructor opens a new handle to the given file.
  ///
  /// For details, see @ref open(const char*,const open_args&)
  ///
  /// @param ex The I/O executor that the file will use, by default, to
  /// dispatch handlers for any asynchronous operations performed on the file.
  ///
  /// @param filename The path of the file to open.
  ///
  /// @param args Additional options used to open the file.
  ///
  /// @param ec Set to indicate what error occurred. If no error occurred,
  /// the object is reset.
  ///
  /// @see open_flags
  basic_file(const executor_type& ex, const char* filename, const open_args& args, error_code& ec)
    : holder_(ex)
  {
    holder_.get_service().open(holder_.get_implementation(), filename, args, ec);
  }

  /// @brief Open a file and construct a basic_file.
  ///
  /// This constructor opens a new handle to the given file.
  ///
  /// For details, see @ref open(const char*,const open_args&)
  ///
  /// @param context An execution context which provides the I/O executor that
  /// the file will use, by default, to dispatch handlers for any asynchronous
  /// operations performed on the file.
  ///
  /// @param filename The path of the file to open.
  ///
  /// @param args Additional options used to open the file.
  ///
  /// @param ec Set to indicate what error occurred. If no error occurred,
  /// the object is reset.
  ///
  /// @see open_flags
  template <execution_context ExecutionContext>
  basic_file(ExecutionContext& context, const char* filename, const open_args& args, error_code& ec)
    : holder_(context)
  {
    holder_.get_service().open(holder_.get_implementation(), filename, args, ec);
  }

#if defined(ASIOEXT_WINDOWS) || defined(ASIOEXT_IS_DOCUMENTATION)
  /// @copydoc basic_file(const executor_type&,const char*,const open_args&)
  ///
  /// @note Only available on Windows.
  basic_file(const executor_type& ex, const wchar_t* filename, const open_args& args)
    : holder_(ex)
  {
    error_code ec;
    holder_.get_service().open(holder_.get_implementation(), filename, args, ec);
    detail::throw_error(ec, "basic_file construct");
  }

  /// @copydoc basic_file(ExecutionContext&,const char*,const open_args&)
  ///
  /// @note Only available on Windows.
  template <execution_context ExecutionContext>
  basic_file(ExecutionContext& context, const wchar_t* filename, const open_args& args)
    : holder_(context)
  {
    error_code ec;
    holder_.get_service().open(holder_.get_implementation(), filename, args, ec);
    detail::throw_error(ec, "basic_file construct");
  }

  /// @copydoc basic_file(const executor_type&,const char*,const open_args&,error_code&)
  ///
  /// @note Only available on Windows.
  basic_file(const executor_type& ex, const wchar_t* filename,
             const open_args& args, error_code& ec)
    : holder_(ex)
  {
    holder_.get_service().open(holder_.get_implementation(), filename, args, ec);
  }

  /// @copydoc basic_file(ExecutionContext&,const char*,const open_args&,error_code&)
  ///
  /// @note Only available on Windows.
  template <execution_context ExecutionContext>
  basic_file(ExecutionContext& context, const wchar_t* filename, const open_args& args, error_code& ec)
    : holder_(context)
  {
    holder_.get_service().open(holder_.get_implementation(), filename, args, ec);
  }
#endif

#if defined(ASIOEXT_HAS_BOOST_FILESYSTEM) || defined(ASIOEXT_IS_DOCUMENTATION)
  /// @copydoc basic_file(const executor_type&,const char*,const open_args&)
  basic_file(const executor_type& ex, const boost::filesystem::path& filename, const open_args& args)
    : holder_(ex)
  {
    error_code ec;
    holder_.get_service().open(holder_.get_implementation(), filename, args, ec);
    detail::throw_error(ec, "basic_file construct");
  }

  /// @copydoc basic_file(ExecutionContext&,const char*,const open_args&)
  template <execution_context ExecutionContext>
  basic_file(ExecutionContext& context, const boost::filesystem::path& filename, const open_args& args)
    : holder_(context)
  {
    error_code ec;
    holder_.get_service().open(holder_.get_implementation(), filename, args, ec);
    detail::throw_error(ec, "basic_file construct");
  }

  /// @copydoc basic_file(const executor_type&,const char*,const open_args&,error_code&)
  basic_file(const executor_type& ex, const boost::filesystem::path& filename, const open_args& args, error_code& ec)
    : holder_(ex)
  {
    holder_.get_service().open(holder_.get_implementation(), filename, args, ec);
  }

  /// @copydoc basic_file(ExecutionContext&,const char*,const open_args&,error_code&)
  template <execution_context ExecutionContext>
  basic_file(ExecutionContext& context, const boost::filesystem::path& filename, const open_args& args, error_code& ec)
    : holder_(context)
  {
    holder_.get_service().open(holder_.get_implementation(), filename, args, ec);
  }
#endif

  /// Get the executor associated with the object.
  const executor_type& get_executor() noexcept
  {
    return holder_.get_executor();
  }

  /// @brief Get a reference to the lowest layer.
  ///
  /// This function returns a reference to the lowest layer in a stack of
  /// layers. Since a basic_file cannot contain any further layers, it simply
  /// returns a reference to itself.
  ///
  /// @return A reference to the lowest layer in the stack of layers. Ownership
  /// is not transferred to the caller.
  lowest_layer_type& lowest_layer() ASIOEXT_NOEXCEPT
  {
    return *this;
  }

  /// @brief Get a const reference to the lowest layer.
  ///
  /// This function returns a const reference to the lowest layer in a stack of
  /// layers. Since a basic_file cannot contain any further layers, it simply
  /// returns a reference to itself.
  ///
  /// @return A const reference to the lowest layer in the stack of layers.
  /// Ownership is not transferred to the caller.
  const lowest_layer_type& lowest_layer() const ASIOEXT_NOEXCEPT
  {
    return *this;
  }

  /// @brief Get the native handle representation.
  ///
  /// This function may be used to obtain the underlying representation of the
  /// handle. This is intended to allow access to native handle functionality
  /// that is not otherwise provided.
  native_handle_type native_handle() ASIOEXT_NOEXCEPT
  {
    return holder_.get_service().native_handle(holder_.get_implementation());
  }

  /// @brief Cancel all asynchronous operations associated with the file.
  ///
  /// This function causes all outstanding asynchronous write and read
  /// operations to finish immediately, and the handlers for cancelled operations
  /// will be passed the asio::error::operation_aborted error.
  ///
  /// @throws asio::system_error Thrown on failure.
  void cancel()
  {
    error_code ec;
    holder_.get_service().cancel(holder_.get_implementation(), ec);
    detail::throw_error(ec, "cancel");
  }

  /// @brief Cancel all asynchronous operations associated with the file.
  ///
  /// This function causes all outstanding asynchronous write and read
  /// operations to finish immediately, and the handlers for cancelled operations
  /// will be passed the asio::error::operation_aborted error.
  ///
  /// @param ec Set to indicate what error occurred. If no error occurred,
  /// the object is reset.
  void cancel(error_code& ec) ASIOEXT_NOEXCEPT
  {
    return holder_.get_service().cancel(holder_.get_implementation(), ec);
  }

  /// @name Handle-management functions
  /// @{

  /// @brief Open a file.
  ///
  /// This function closes any currently held handle and attempts to open
  /// a handle to the specified file.
  ///
  /// For details, see @ref open(const char*,const open_args&)
  ///
  /// @param filename The path of the file to open.
  /// See @ref filenames for details.
  ///
  /// @param args Additional options used to open the file.
  ///
  /// @throws asio::system_error Thrown on failure.
  ///
  /// @see open_flags
  void open(const char* filename, const open_args& args)
  {
    error_code ec;
    holder_.get_service().open(holder_.get_implementation(), filename, args, ec);
    detail::throw_error(ec, "open");
  }

  /// @brief Open a file.
  ///
  /// This function closes any currently held handle and attempts to open
  /// a handle to the specified file.
  ///
  /// For details, see @ref open(const char*,const open_args&)
  ///
  /// @param filename The path of the file to open.
  /// See @ref filenames for details.
  ///
  /// @param args Additional options used to open the file.
  ///
  /// @param ec Set to indicate what error occurred. If no error occurred,
  /// the object is reset.
  ///
  /// @see open_flags
  void open(const char* filename, const open_args& args,
            error_code& ec) ASIOEXT_NOEXCEPT
  {
    holder_.get_service().open(holder_.get_implementation(), filename, args, ec);
  }

#if defined(ASIOEXT_WINDOWS) || defined(ASIOEXT_IS_DOCUMENTATION)
  /// @copydoc open(const char*,const open_args&)
  ///
  /// @note Only available on Windows.
  void open(const wchar_t* filename, const open_args& args)
  {
    error_code ec;
    holder_.get_service().open(holder_.get_implementation(), filename, args, ec);
    detail::throw_error(ec, "open");
  }

  /// @copydoc open(const char*,const open_args&,error_code&)
  ///
  /// @note Only available on Windows.
  void open(const wchar_t* filename, const open_args& args,
            error_code& ec) ASIOEXT_NOEXCEPT
  {
    holder_.get_service().open(holder_.get_implementation(), filename, args, ec);
  }
#endif

#if defined(ASIOEXT_HAS_BOOST_FILESYSTEM) || defined(ASIOEXT_IS_DOCUMENTATION)
  /// @copydoc open(const char*,const open_args&)
  ///
  /// @note Only available if using Boost.Filesystem
  /// (i.e. if @c ASIOEXT_HAS_BOOST_FILESYSTEM is defined)
  void open(const boost::filesystem::path& filename, const open_args& args)
  {
    error_code ec;
    holder_.get_service().open(holder_.get_implementation(), filename, args, ec);
    detail::throw_error(ec, "open");
  }

  /// @copydoc open(const char*,const open_args&,error_code&)
  ///
  /// @note Only available if using Boost.Filesystem
  /// (i.e. if @c ASIOEXT_HAS_BOOST_FILESYSTEM is defined)
  void open(const boost::filesystem::path& filename, const open_args& args,
            error_code& ec) ASIOEXT_NOEXCEPT
  {
    holder_.get_service().open(holder_.get_implementation(), filename, args, ec);
  }
#endif

  /// Determine whether the handle is open.
  bool is_open() const ASIOEXT_NOEXCEPT
  {
    return holder_.get_service().is_open(holder_.get_implementation());
  }

  /// @brief Close the handle.
  ///
  /// This function is used to close the handle.
  ///
  /// @throws asio::system_error Thrown on failure.
  void close()
  {
    error_code ec;
    holder_.get_service().close(holder_.get_implementation(), ec);
    detail::throw_error(ec, "close");
  }

  /// @brief Close the handle.
  ///
  /// This function is used to close the handle.
  ///
  /// @param ec Set to indicate what error occurred. If no error occurred,
  /// the object is reset.
  void close(error_code& ec) ASIOEXT_NOEXCEPT
  {
    holder_.get_service().close(holder_.get_implementation(), ec);
  }

  /// @}

  /// @name File pointer functions
  /// @{

  /// @brief Get the current file position.
  ///
  /// This function retrieves the current read/write position of this
  /// file handle, relative to the file's beginning.
  ///
  /// @return The current absolute file position.
  ///
  /// @throws asio::system_error Thrown on failure.
  uint64_t position()
  {
    error_code ec;
    uint64_t p = holder_.get_service().position(holder_.get_implementation(), ec);
    detail::throw_error(ec, "position");
    return p;
  }

  /// @brief Get the current file position.
  ///
  /// This function retrieves the current read/write position of this
  /// file handle, relative to the file's beginning.
  ///
  /// @return The current absolute file position.
  ///
  /// @param ec Set to indicate what error occurred. If no error occurred,
  /// the object is reset.
  uint64_t position(error_code& ec) ASIOEXT_NOEXCEPT
  {
    return holder_.get_service().position(holder_.get_implementation(), ec);
  }

  /// @brief Change the read/write position.
  ///
  /// This function repositions the current read/write position of this
  /// basic_file by @c offset, relative to the origin specified
  /// by @c origin.
  ///
  /// @param origin The origin of @c offset.
  ///
  /// @param offset The offset by which the current position's modified.
  ///
  /// @return The new absolute file position.
  ///
  /// @throws asio::system_error Thrown on failure.
  uint64_t seek(seek_origin origin, int64_t offset)
  {
    error_code ec;
    uint64_t p = holder_.get_service().seek(holder_.get_implementation(), origin,
                                          offset, ec);
    detail::throw_error(ec, "seek");
    return p;
  }

  /// @brief Change the read/write position.
  ///
  /// This function repositions the current read/write position of this
  /// basic_file by @c offset, relative to the origin specified
  /// by @c origin.
  ///
  /// @param origin The origin of @c offset.
  ///
  /// @param offset The offset by which the current position's modified.
  ///
  /// @return The new absolute file position.
  ///
  /// @param ec Set to indicate what error occurred. If no error occurred,
  /// the object is reset.
  uint64_t seek(seek_origin origin, int64_t offset,
                error_code& ec) ASIOEXT_NOEXCEPT
  {
    return holder_.get_service().seek(holder_.get_implementation(), origin, offset,
                                    ec);
  }

  /// @}

  /// @name Metadata functions
  /// @{

  /// @copydoc file_handle::size()
  uint64_t size()
  {
    error_code ec;
    uint64_t s = holder_.get_service().size(holder_.get_implementation(), ec);
    detail::throw_error(ec, "size");
    return s;
  }

  /// @copydoc file_handle::size(error_code&)
  uint64_t size(error_code& ec) ASIOEXT_NOEXCEPT
  {
    return holder_.get_service().size(holder_.get_implementation(), ec);
  }

  /// @copydoc file_handle::truncate(uint64_t)
  void truncate(uint64_t new_size)
  {
    error_code ec;
    holder_.get_service().truncate(holder_.get_implementation(), new_size, ec);
    detail::throw_error(ec, "truncate");
  }

  /// @copydoc file_handle::truncate(uint64_t,error_code&)
  void truncate(uint64_t new_size, error_code& ec) ASIOEXT_NOEXCEPT
  {
    holder_.get_service().truncate(holder_.get_implementation(), new_size, ec);
  }

  /// @copydoc file_handle::permissions()
  ASIOEXT_WINDOWS_NO_HANDLEINFO_WARNING
  file_perms permissions()
  {
    error_code ec;
    file_perms p = holder_.get_service().permissions(holder_.get_implementation(),
                                                   ec);
    detail::throw_error(ec, "get_permissions");
    return p;
  }

  /// @copydoc file_handle::permissions(error_code&)
  ASIOEXT_WINDOWS_NO_HANDLEINFO_WARNING
  file_perms permissions(error_code& ec) ASIOEXT_NOEXCEPT
  {
    return holder_.get_service().permissions(holder_.get_implementation(), ec);
  }

  /// @copydoc file_handle::permissions(file_perms,file_perm_options)
  ASIOEXT_WINDOWS_NO_HANDLEINFO_WARNING
  void permissions(file_perms perms, file_perm_options opts =
                    file_perm_options::replace)
  {
    error_code ec;
    holder_.get_service().permissions(holder_.get_implementation(), perms,
                                    opts, ec);
    detail::throw_error(ec, "set_permissions");
  }

  /// @copydoc file_handle::permissions(file_perms,error_code&)
  ASIOEXT_WINDOWS_NO_HANDLEINFO_WARNING
  void permissions(file_perms perms, error_code& ec) ASIOEXT_NOEXCEPT
  {
    holder_.get_service().permissions(holder_.get_implementation(), perms, ec);
  }

  /// @copydoc file_handle::permissions(file_perms,file_perm_options,error_code&)
  ASIOEXT_WINDOWS_NO_HANDLEINFO_WARNING
  void permissions(file_perms perms, file_perm_options opts,
                   error_code& ec) ASIOEXT_NOEXCEPT
  {
    holder_.get_service().permissions(holder_.get_implementation(), perms,
                                    opts, ec);
  }

  /// @copydoc file_handle::attributes()
  ASIOEXT_WINDOWS_NO_HANDLEINFO_WARNING
  file_attrs attributes()
  {
    error_code ec;
    file_attrs a = holder_.get_service().attributes(holder_.get_implementation(), ec);
    detail::throw_error(ec, "get_attributes");
    return a;
  }

  /// @copydoc file_handle::attributes(error_code&)
  ASIOEXT_WINDOWS_NO_HANDLEINFO_WARNING
  file_attrs attributes(error_code& ec) ASIOEXT_NOEXCEPT
  {
    return holder_.get_service().attributes(holder_.get_implementation(), ec);
  }

  /// @copydoc file_handle::attributes(file_attrs,file_attr_options)
  ASIOEXT_WINDOWS_NO_HANDLEINFO_WARNING
  void attributes(file_attrs attrs, file_attr_options opts =
                    file_attr_options::replace)
  {
    error_code ec;
    holder_.get_service().attributes(holder_.get_implementation(),
                                   attrs, opts, ec);
    detail::throw_error(ec, "set_attributes");
  }

  /// @copydoc file_handle::attributes(file_attrs,error_code&)
  ASIOEXT_WINDOWS_NO_HANDLEINFO_WARNING
  void attributes(file_attrs attrs, error_code& ec) ASIOEXT_NOEXCEPT
  {
    holder_.get_service().attributes(holder_.get_implementation(),
                                   attrs, ec);
  }

  /// @copydoc file_handle::attributes(file_attrs,file_attr_options,error_code&)
  ASIOEXT_WINDOWS_NO_HANDLEINFO_WARNING
  void attributes(file_attrs attrs, file_attr_options opts,
                  error_code& ec) ASIOEXT_NOEXCEPT
  {
    holder_.get_service().attributes(holder_.get_implementation(),
                                   attrs, opts, ec);
  }

  /// @copydoc file_handle::times()
  file_times times()
  {
    error_code ec;
    file_times t = holder_.get_service().times(holder_.get_implementation(), ec);
    detail::throw_error(ec, "get_times");
    return t;
  }

  /// @copydoc file_handle::times(error_code&)
  file_times times(error_code& ec) ASIOEXT_NOEXCEPT
  {
    return holder_.get_service().times(holder_.get_implementation(), ec);
  }

  /// @copydoc file_handle::times(const file_times&)
  void times(const file_times& new_times)
  {
    error_code ec;
    holder_.get_service().times(holder_.get_implementation(), new_times, ec);
    detail::throw_error(ec, "set_times");
  }

  /// @copydoc file_handle::times(const file_times&,error_code&)
  void times(const file_times& new_times, error_code& ec) ASIOEXT_NOEXCEPT
  {
    holder_.get_service().times(holder_.get_implementation(), new_times, ec);
  }

  /// @}

  /// @name SyncReadStream functions
  /// @{

  /// @brief Read some data from the file.
  ///
  /// This function is used to read data from the file, starting at the
  /// basic_file's current file position. The function call will
  /// block until one or more bytes of data has been read successfully,
  /// or until an error occurs.
  ///
  /// @param buffers One or more buffers into which the data will be read.
  ///
  /// @returns The number of bytes read.
  ///
  /// @throws asio::system_error Thrown on failure. An error code of
  /// asio::error::eof indicates that the end of file was reached.
  ///
  /// @note The read_some operation may not read all of the requested number of
  /// bytes. Consider using the @c asio::read function if you need to ensure
  /// that the requested amount of data is read before the blocking operation
  /// completes.
  ///
  /// @par Example
  /// To read into a single data buffer use the @c asio::buffer function as
  /// follows:
  /// @code
  /// fh.read_some(asio::buffer(data, size));
  /// @endcode
  /// See the @c asio::buffer documentation for information on reading into
  /// multiple buffers in one go, and how to use it with arrays, boost::array
  /// or std::vector.
  template <typename MutableBufferSequence>
  std::size_t read_some(const MutableBufferSequence& buffers)
  {
    error_code ec;
    const std::size_t s =
        holder_.get_service().read_some(holder_.get_implementation(), buffers, ec);
    detail::throw_error(ec, "read_some");
    return s;
  }

  /// @brief Read some data from the file.
  ///
  /// This function is used to read data from the file, starting at the
  /// basic_file's current file position. The function call will
  /// block until one or more bytes of data has been read successfully,
  /// or until an error occurs.
  ///
  /// @param buffers One or more buffers into which the data will be read.
  ///
  /// @param ec Set to indicate what error occurred. If no error occurred,
  /// the object is reset.
  ///
  /// @returns The number of bytes read. Returns 0 if an error occurred.
  ///
  /// @note The read_some operation may not read all of the requested number of
  /// bytes. Consider using the @c asio::read function if you need to ensure
  /// that the requested amount of data is read before the blocking operation
  /// completes.
  template <typename MutableBufferSequence>
  std::size_t read_some(const MutableBufferSequence& buffers,
                        error_code& ec) ASIOEXT_NOEXCEPT
  {
    return holder_.get_service().read_some(holder_.get_implementation(), buffers,
                                         ec);
  }

  /// @}

  /// @name SyncWriteStream functions
  /// @{

  /// @brief Write some data to the file.
  ///
  /// This function is used to write data to the file, starting at the
  /// basic_file's current file position. The function call will
  /// block until one or more bytes of the data has been written
  /// successfully, or until an error occurs.
  ///
  /// @param buffers One or more data buffers to be written to the file.
  ///
  /// @returns The number of bytes written.
  ///
  /// @throws asio::system_error Thrown on failure.
  ///
  /// @note The write_some operation may not write all of the data.
  /// Consider using the @c asio::write function if you need to ensure
  /// that all data is written before the blocking operation completes.
  ///
  /// @par Example
  /// To write a single data buffer use the @c asio::buffer function as follows:
  /// @code
  /// fh.write_some(asio::buffer(data, size));
  /// @endcode
  /// See the @c asio::buffer documentation for information on writing multiple
  /// buffers in one go, and how to use it with arrays, boost::array or
  /// std::vector.
  template <typename ConstBufferSequence>
  std::size_t write_some(const ConstBufferSequence& buffers)
  {
    error_code ec;
    const std::size_t s =
        holder_.get_service().write_some(holder_.get_implementation(), buffers, ec);
    detail::throw_error(ec, "write_some");
    return s;
  }

  /// @brief Write some data to the file.
  ///
  /// This function is used to write data to the file, starting at the
  /// basic_file's current file position. The function call will
  /// block until one or more bytes of the data has been written
  /// successfully, or until an error occurs.
  ///
  /// @param buffers One or more data buffers to be written to the file.
  ///
  /// @param ec Set to indicate what error occurred. If no error occurred,
  /// the object is reset.
  ///
  /// @returns The number of bytes written. Returns 0 if an error occurred.
  ///
  /// @note The write_some operation may not write all of the data.
  /// Consider using the @c asio::write function if you need to ensure
  /// that all data is written before the blocking operation completes.
  template <typename ConstBufferSequence>
  std::size_t write_some(const ConstBufferSequence& buffers,
                         error_code& ec) ASIOEXT_NOEXCEPT
  {
    return holder_.get_service().write_some(holder_.get_implementation(), buffers,
                                          ec);
  }

  /// @}

  /// @name SyncRandomAccessReadDevice functions
  /// @{

  /// @brief Read some data from the file at the specified offset.
  ///
  /// This function is used to read data from the file. The
  /// function call will block until one or more bytes of data has been read
  /// successfully, or until an error occurs.
  ///
  /// @param offset The offset at which the data will be read, relative to
  /// the file's beginning.
  ///
  /// @param buffers One or more buffers into which the data will be read.
  ///
  /// @returns The number of bytes read.
  ///
  /// @throws asio::system_error Thrown on failure. An error code of
  /// asio::error::eof indicates that the end of the file was reached.
  ///
  /// @note The read_some operation may not read all of the requested number of
  /// bytes. Consider using the @c asio::read_at function if you need to ensure
  /// that the requested amount of data is read before the blocking operation
  /// completes.
  ///
  /// @par Example
  /// To read into a single data buffer use the @c asio::buffer function as
  /// follows:
  /// @code
  /// handle.read_some_at(42, asio::buffer(data, size));
  /// @endcode
  /// See the @c asio::buffer documentation for information on reading into
  /// multiple buffers in one go, and how to use it with arrays, boost::array
  /// or std::vector.
  template <typename MutableBufferSequence>
  std::size_t read_some_at(uint64_t offset,
                           const MutableBufferSequence& buffers)
  {
    error_code ec;
    const std::size_t s = holder_.get_service().read_some_at(
        holder_.get_implementation(), offset, buffers, ec);
    detail::throw_error(ec, "read_some_at");
    return s;
  }

  /// @brief Read some data from the file at the specified offset.
  ///
  /// This function is used to read data from the file. The
  /// function call will block until one or more bytes of data has been read
  /// successfully, or until an error occurs.
  ///
  /// @param offset The offset at which the data will be read, relative to
  /// the file's beginning.
  ///
  /// @param buffers One or more buffers into which the data will be read.
  ///
  /// @param ec Set to indicate what error occurred. If no error occurred,
  /// the object is reset.
  ///
  /// @returns The number of bytes read. Returns 0 if an error occurred.
  ///
  /// @note The read_some operation may not read all of the requested number of
  /// bytes. Consider using the @c asio::read_at function if you need to ensure
  /// that the requested amount of data is read before the blocking operation
  /// completes.
  template <typename MutableBufferSequence>
  std::size_t read_some_at(uint64_t offset,
                           const MutableBufferSequence& buffers,
                           error_code& ec) ASIOEXT_NOEXCEPT
  {
    return holder_.get_service().read_some_at(holder_.get_implementation(), offset,
                                            buffers, ec);
  }

  /// @}

  /// @name SyncRandomAccessWriteDevice functions
  /// @{

  /// @brief Write some data to the file at the specified offset.
  ///
  /// This function is used to write data to the file. The
  /// function call will block until one or more bytes of the data has been
  /// written successfully, or until an error occurs.
  ///
  /// @param offset The offset at which the data will be written, relative to
  /// the file's beginning.
  ///
  /// @param buffers One or more data buffers to be written.
  ///
  /// @returns The number of bytes written.
  ///
  /// @throws asio::system_error Thrown on failure.
  ///
  /// @note The write_some_at operation may not write all of the data. Consider
  /// using the @c asio::write_at function if you need to ensure that all data
  /// is written before the blocking operation completes.
  ///
  /// @par Example
  /// To write a single data buffer use the @c asio::buffer function as follows:
  /// @code
  /// fh.write_some_at(42, asio::buffer(data, size));
  /// @endcode
  /// See the @c asio::buffer documentation for information on writing multiple
  /// buffers in one go, and how to use it with arrays, boost::array or
  /// std::vector.
  template <typename ConstBufferSequence>
  std::size_t write_some_at(uint64_t offset, const ConstBufferSequence& buffers)
  {
    error_code ec;
    const std::size_t s = holder_.get_service().write_some_at(
        holder_.get_implementation(), offset, buffers, ec);
    detail::throw_error(ec, "write_some_at");
    return s;
  }

  /// @brief Write some data to the file at the specified offset.
  ///
  /// This function is used to write data to the file. The
  /// function call will block until one or more bytes of the data has been
  /// written successfully, or until an error occurs.
  ///
  /// @param offset The offset at which the data will be written, relative to
  /// the file's beginning.
  ///
  /// @param buffers One or more data buffers to be written to the handle.
  ///
  /// @param ec Set to indicate what error occurred. If no error occurred,
  /// the object is reset.
  ///
  /// @returns The number of bytes written. Returns 0 if an error occurred.
  ///
  /// @note The write_some_at operation may not write all of the data. Consider
  /// using the @c asio::write_at function if you need to ensure that all data
  /// is written before the blocking operation completes.
  template <typename ConstBufferSequence>
  std::size_t write_some_at(uint64_t offset,
                            const ConstBufferSequence& buffers,
                            error_code& ec) ASIOEXT_NOEXCEPT
  {
    return holder_.get_service().write_some_at(holder_.get_implementation(), offset,
                                             buffers, ec);
  }

  /// @}

  /// @name AsyncReadStream functions
  /// @{

  /// Start an asynchronous read.
  ///
  /// This function is used to asynchronously read data from the file.
  /// The function call always returns immediately.
  ///
  /// @param buffers One or more buffers into which the data will be read.
  /// Although the buffers object may be copied as necessary, ownership of the
  /// underlying memory blocks is retained by the caller, which must guarantee
  /// that they remain valid until the handler is called.
  ///
  /// @param handler The handler to be called when the read operation completes.
  /// Copies will be made of the handler as required. The function signature of
  /// the handler must be:
  /// @code void handler(
  ///   const error_code& error, // Result of operation.
  ///   std::size_t bytes_transferred // Number of bytes read.
  /// ); @endcode
  /// Regardless of whether the asynchronous operation completes immediately or
  /// not, the handler will not be invoked from within this function. Invocation
  /// of the handler will be performed in a manner equivalent to using
  /// asio::io_context::post().
  ///
  /// @note The read operation may not read all of the requested number of bytes.
  /// Consider using the @c asio::async_read function if you need to ensure
  /// that the requested amount of data is read before the asynchronous
  /// operation completes.
  ///
  /// @par Example
  /// To read into a single data buffer use the @c asio::buffer function as
  /// follows: @code
  /// file.async_read_some(asio::buffer(data, size), handler);
  /// @endcode
  /// See the asio::buffer documentation for information on reading into
  /// multiple buffers in one go, and how to use it with arrays, boost::array
  /// or std::vector.
  ///
  /// @warning Interleaving asynchronous reads with other operations that cause
  /// the file position/pointer to change leads to undefined behaviour!
  ///
  /// @note Starting multiple asynchronous reads is implementation-defined and
  /// not recommended. (A FileService might make additional gurantees, but is
  /// not required to.)
  template <typename MutableBufferSequence, typename ReadHandler>
  ASIOEXT_INITFN_RESULT_TYPE(ReadHandler, void (error_code, std::size_t))
  async_read_some(const MutableBufferSequence& buffers,
                  ReadHandler&& handler)
  {
    return holder_.get_service().async_read_some(holder_.get_implementation(),
        buffers, std::forward<ReadHandler>(handler));
  }

  /// @}

  /// @name AsyncWriteStream functions
  /// @{

  /// @brief Start an asynchronous write.
  ///
  /// This function is used to asynchronously write data to the file.
  /// The function call always returns immediately.
  ///
  /// @param buffers One or more data buffers to be written to the file.
  /// Although the buffers object may be copied as necessary, ownership of the
  /// underlying memory blocks is retained by the caller, which must guarantee
  /// that they remain valid until the handler is called.
  ///
  /// @param handler The handler to be called when the write operation completes.
  /// Copies will be made of the handler as required. The function signature of
  /// the handler must be:
  /// @code void handler(
  ///   const error_code& error, // Result of operation.
  ///   std::size_t bytes_transferred // Number of bytes written.
  /// ); @endcode
  /// Regardless of whether the asynchronous operation completes immediately or
  /// not, the handler will not be invoked from within this function. Invocation
  /// of the handler will be performed in a manner equivalent to using
  /// asio::io_context::post().
  ///
  /// @note The write operation may not write all of the data.
  /// Consider using the @c asio::async_write function if you need to ensure
  /// that all data is written before the asynchronous operation completes.
  ///
  /// @par Example
  /// To write a single data buffer use the @c asio::buffer function as
  /// follows: @code
  /// file.async_write_some(asio::buffer(data, size), handler);
  /// @endcode
  /// See the asio::buffer documentation for information on writing multiple
  /// buffers in one go, and how to use it with arrays, boost::array or
  /// std::vector.
  ///
  /// @warning Interleaving asynchronous writes with other operations that
  /// cause the file position/pointer to change leads to undefined behaviour!
  ///
  /// @note Starting multiple asynchronous writes is implementation-defined and
  /// not recommended. (A FileService might make additional gurantees, but is
  /// not required to.)
  template <typename ConstBufferSequence, typename WriteHandler>
  ASIOEXT_INITFN_RESULT_TYPE(WriteHandler, void (error_code, std::size_t))
  async_write_some(const ConstBufferSequence& buffers,
                   WriteHandler&& handler)
  {
    return holder_.get_service().async_write_some(holder_.get_implementation(),
        buffers, std::forward<WriteHandler>(handler));
  }

  /// @}

  /// @name AsyncRandomAccessReadDevice functions
  /// @{

  /// @brief Start an asynchronous read at the specified offset.
  ///
  /// This function is used to asynchronously read data from the file.
  /// The function call always returns immediately.
  ///
  /// @param offset The offset at which the data will be read.
  ///
  /// @param buffers One or more buffers into which the data will be read.
  /// Although the buffers object may be copied as necessary, ownership of the
  /// underlying memory blocks is retained by the caller, which must guarantee
  /// that they remain valid until the handler is called.
  ///
  /// @param handler The handler to be called when the read operation completes.
  /// Copies will be made of the handler as required. The function signature of
  /// the handler must be:
  /// @code void handler(
  ///   const error_code& error, // Result of operation.
  ///   std::size_t bytes_transferred // Number of bytes read.
  /// ); @endcode
  /// Regardless of whether the asynchronous operation completes immediately or
  /// not, the handler will not be invoked from within this function. Invocation
  /// of the handler will be performed in a manner equivalent to using
  /// asio::io_context::post().
  ///
  /// @note The read operation may not read all of the requested number of
  /// bytes.
  /// Consider using the asio::async_read_at function if you need to ensure that
  /// the requested amount of data is read before the asynchronous operation
  /// completes.
  ///
  /// @par Example
  /// To read into a single data buffer use the asio::buffer function as
  /// follows:
  /// @code
  /// file.async_read_some_at(42, asio::buffer(data, size), handler);
  /// @endcode
  /// See the asio::buffer documentation for information on reading into
  /// multiple
  /// buffers in one go, and how to use it with arrays, boost::array or
  /// std::vector.
  template <typename MutableBufferSequence, typename ReadHandler>
  ASIOEXT_INITFN_RESULT_TYPE(ReadHandler, void(error_code, std::size_t))
  async_read_some_at(uint64_t offset,
                     const MutableBufferSequence& buffers,
                     ReadHandler&& handler)
  {
    return holder_.get_service().async_read_some_at(holder_.get_implementation(),
        offset, buffers, std::forward<ReadHandler>(handler));
  }

  /// @}

  /// @name AsyncRandomAccessWriteDevice functions
  /// @{

  /// @brief Start an asynchronous write at the specified offset.
  ///
  /// This function is used to asynchronously write data to the file.
  /// The function call always returns immediately.
  ///
  /// @param offset The offset at which the data will be written.
  ///
  /// @param buffers One or more data buffers to be written to the file.
  /// Although the buffers object may be copied as necessary, ownership of the
  /// underlying memory blocks is retained by the caller, which must guarantee
  /// that they remain valid until the handler is called.
  ///
  /// @param handler The handler to be called when the write operation
  /// completes.
  /// Copies will be made of the handler as required. The function signature of
  /// the handler must be:
  /// @code void handler(
  ///   const error_code& error, // Result of operation.
  ///   std::size_t bytes_transferred // Number of bytes written.
  /// ); @endcode
  /// Regardless of whether the asynchronous operation completes immediately or
  /// not, the handler will not be invoked from within this function. Invocation
  /// of the handler will be performed in a manner equivalent to using
  /// asio::io_context::post().
  ///
  /// @note The write operation may not write all of the data.
  /// Consider using the asio::async_write_at function if you need to ensure
  /// that
  /// all data is written before the asynchronous operation completes.
  ///
  /// @par Example
  /// To write a single data buffer use the asio::buffer function as follows:
  /// @code
  /// file.async_write_some_at(42, asio::buffer(data, size), handler);
  /// @endcode
  /// See the asio::buffer documentation for information on writing multiple
  /// buffers in one go, and how to use it with arrays, boost::array or
  /// std::vector.
  template <typename ConstBufferSequence, typename WriteHandler>
  ASIOEXT_INITFN_RESULT_TYPE(WriteHandler, void(error_code, std::size_t))
  async_write_some_at(uint64_t offset,
                      const ConstBufferSequence& buffers,
                      WriteHandler&& handler)
  {
    return holder_.get_service().async_write_some_at(holder_.get_implementation(),
        offset, buffers, std::forward<WriteHandler>(handler));
  }

  /// @}

private:
  io_object_holder<FileService, Executor> holder_;
};

ASIOEXT_NS_END

#endif
