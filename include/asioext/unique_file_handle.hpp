/// @file
/// Defines the unique_file_handle class
///
/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_UNIQUEFILEHANDLE_HPP
#define ASIOEXT_UNIQUEFILEHANDLE_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include "asioext/file_handle.hpp"
#include "asioext/file_perms.hpp"
#include "asioext/file_attrs.hpp"
#include "asioext/seek_origin.hpp"
#include "asioext/error_code.hpp"

ASIOEXT_NS_BEGIN

/// @ingroup files_handle
/// @brief RAII wrapper around file_handle.
///
/// The unique_file_handle class provides a std::unique_ptr<>-like wrapper
/// around a file_handle. The unique_file_handle owns the contained native
/// handle.
///
/// unique_file_handle objects cannot be copied, but are move-constructible/
/// move-assignable if a compiler with C++11 support is used.
///
/// unique_file_handle models the following asio concepts:
/// * SyncRandomAccessReadDevice
/// * SyncRandomAccessWriteDevice
/// * SyncReadStream
/// * SyncWriteStream
///
/// @par Thread Safety:
/// @e Distinct @e objects: Safe.@n
/// @e Shared @e objects: Unsafe.
///
/// @note If copying a handle (which is a costly operation) is really necessary,
/// the @ref duplicate function can be used.
///
/// @par Example
/// @code
/// asioext::unique_file_handle file = asioext::open(
///     "myfile.txt", asioext::open_flags::access_read_write |
///                   asioext::open_flags::create_always);
///
/// // Write a string
/// const std::string content = "Hello world";
/// asio::write(file, asio::buffer(content));
///
/// // Read it back
/// // Note that we could avoid seek() by using write_at/read_t!
/// file.seek(asioext::seek_origin::from_begin, 0);
/// std::string content_returned(content.size());
/// asio::read(file, asio::buffer(content));
/// @endcode
class unique_file_handle
{
public:
  /// A unique_file_handle is always the lowest layer.
  typedef unique_file_handle lowest_layer_type;

  /// @brief Construct an empy unique_file_handle.
  ///
  /// This constructor initializes the unique_file_handle to an empty state.
  ASIOEXT_DECL unique_file_handle() ASIOEXT_NOEXCEPT;

  /// Destroy a unique_file_handle.
  ///
  /// This destructor attempts to close the currently owned file handle.
  /// Failures are silently ignored.
  ASIOEXT_DECL ~unique_file_handle();

  /// @brief Construct a unique_file_handle using a file_handle object.
  ///
  /// This constructor takes ownership of the given file_handle.
  ///
  /// @param handle The file_handle object which shall be assigned to this
  /// unique_file_handle object.
  ASIOEXT_DECL explicit unique_file_handle(file_handle handle) ASIOEXT_NOEXCEPT;

  /// @brief Construct a unique_file_handle using a native handle object.
  ///
  /// This constructor takes ownership of the given native handle.
  ///
  /// @param handle The native handle object which shall be assigned to this
  /// unique_file_handle object.
  ASIOEXT_DECL explicit unique_file_handle(
      const file_handle::native_handle_type& handle) ASIOEXT_NOEXCEPT;

#ifdef ASIOEXT_HAS_MOVE
  /// @brief Move-construct a unique_file_handle from another.
  ///
  /// This constructor moves a handle from one object to another.
  ///
  /// @param other The other unique_file_handle object from which the move will
  /// occur.
  ///
  /// @note Following the move, the moved-from object is in the same state as if
  /// constructed using the @c unique_file_handle() constructor.
  ASIOEXT_DECL unique_file_handle(unique_file_handle&& other) ASIOEXT_NOEXCEPT;

  /// @brief Move-assign a unique_file_handle from another.
  ///
  /// This assignment operator moves a handle from one object to another.
  /// If this object already owns a file handle, the current handle will be
  /// closed. This operation is potentially throwing.
  ///
  /// @param other The other unique_file_handle object from which the move will
  /// occur.
  ///
  /// @note Following the move, the moved-from object is in the same state as if
  /// constructed using the @c unique_file_handle() constructor.
  ASIOEXT_DECL unique_file_handle& operator=(unique_file_handle&& other);
#endif

  /// @brief Get a reference to the lowest layer.
  ///
  /// This function returns a reference to the lowest layer in a stack of
  /// layers. Since a unique_file_handle cannot contain any further layers, it
  /// simply returns a reference to itself.
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
  /// layers. Since a unique_file_handle cannot contain any further layers, it
  /// simply returns a reference to itself.
  ///
  /// @return A const reference to the lowest layer in the stack of layers.
  /// Ownership is not transferred to the caller.
  const lowest_layer_type& lowest_layer() const ASIOEXT_NOEXCEPT
  {
    return *this;
  }

  /// @brief Get the managed file_handle.
  ///
  /// This function returns a copy of the managed file_handle.
  ///
  /// @return A copy of the contained file_handle object.
  /// Ownership is not transferred to the caller.
  file_handle get() ASIOEXT_NOEXCEPT
  {
    return handle_;
  }

  /// @copydoc file_handle::is_open()
  bool is_open() const ASIOEXT_NOEXCEPT
  {
    return handle_.is_open();
  }

  /// @copydoc file_handle::close()
  void close()
  {
    handle_.close();
  }

  /// @copydoc file_handle::close(error_code&)
  void close(error_code& ec) ASIOEXT_NOEXCEPT
  {
    handle_.close(ec);
  }

  /// @brief Take ownership of the contained file_handle.
  ///
  /// This function transfers ownership of the contained file_handle to
  /// the caller. The unique_file_handle object is reset to an empty state
  /// (i.e. no file opened).
  ///
  /// @return The contained file_handle.
  /// The caller is now responsible to close it.
  ///
  /// @warning This function is dangerous. It is highly unlikely that you'll
  /// ever need to use this.
  ASIOEXT_DECL file_handle release() ASIOEXT_NOEXCEPT;

  /// @brief Replace the managed file_handle.
  ///
  /// This function closes the currently managed handle (if any),
  /// and replaces it with the given one.
  ///
  /// @param handle The new file_handle to manage.
  /// Ownership is transferred to the unique_file_handle.
  ///
  /// @throws asio::system_error Thrown on failure.
  ASIOEXT_DECL void reset(file_handle handle);

  /// @brief Replace the managed file_handle.
  ///
  /// This function closes the currently managed handle (if any),
  /// and replaces it with the given one.
  ///
  /// @param handle The new file_handle to manage.
  /// Ownership is transferred to the unique_file_handle.
  ///
  /// @param ec Set to indicate what error occurred. If no error occurred,
  /// the object is reset.
  ASIOEXT_DECL void reset(file_handle handle, error_code& ec) ASIOEXT_NOEXCEPT;

  /// @name Positioning functions
  /// @{

  /// @copydoc file_handle::position()
  uint64_t position()
  {
    return handle_.position();
  }

  /// @copydoc file_handle::position(error_code&)
  uint64_t position(error_code& ec) ASIOEXT_NOEXCEPT
  {
    return handle_.position(ec);
  }

  /// @copydoc file_handle::seek(seek_origin,int64_t)
  uint64_t seek(seek_origin origin, int64_t offset)
  {
    return handle_.seek(origin, offset);
  }

  /// @copydoc file_handle::seek(seek_origin,int64_t,error_code&)
  uint64_t seek(seek_origin origin,
                int64_t offset,
                error_code& ec) ASIOEXT_NOEXCEPT
  {
    return handle_.seek(origin, offset, ec);
  }

  /// @}

  /// @name Metadata functions
  /// @{

  /// @copydoc file_handle::size()
  uint64_t size()
  {
    return handle_.size();
  }

  /// @copydoc file_handle::size(error_code&)
  uint64_t size(error_code& ec) ASIOEXT_NOEXCEPT
  {
    return handle_.size(ec);
  }

  /// @copydoc file_handle::size(uint64_t)
  void size(uint64_t new_size)
  {
    handle_.size(new_size);
  }

  /// @copydoc file_handle::size(uint64_t,error_code&)
  void size(uint64_t new_size, error_code& ec) ASIOEXT_NOEXCEPT
  {
    handle_.size(new_size, ec);
  }

  /// @copydoc file_handle::permissions()
  ASIOEXT_WINDOWS_NO_HANDLEINFO_WARNING
  file_perms permissions()
  {
    return handle_.permissions();
  }

  /// @copydoc file_handle::permissions(error_code&)
  ASIOEXT_WINDOWS_NO_HANDLEINFO_WARNING
  file_perms permissions(error_code& ec) ASIOEXT_NOEXCEPT
  {
    return handle_.permissions(ec);
  }

  /// @copydoc file_handle::permissions(file_perms,file_perm_options)
  ASIOEXT_WINDOWS_NO_HANDLEINFO_WARNING
  void permissions(file_perms perms, file_perm_options opts =
                    file_perm_options::replace)
  {
    handle_.permissions(perms, opts);
  }

  /// @copydoc file_handle::permissions(file_perms,error_code&)
  ASIOEXT_WINDOWS_NO_HANDLEINFO_WARNING
  void permissions(file_perms perms, error_code& ec) ASIOEXT_NOEXCEPT
  {
    handle_.permissions(perms, ec);
  }

  /// @copydoc file_handle::permissions(file_perms,file_perm_options,error_code&)
  ASIOEXT_WINDOWS_NO_HANDLEINFO_WARNING
  void permissions(file_perms perms, file_perm_options opts,
                   error_code& ec) ASIOEXT_NOEXCEPT
  {
    handle_.permissions(perms, opts, ec);
  }

  /// @copydoc file_handle::attributes()
  ASIOEXT_WINDOWS_NO_HANDLEINFO_WARNING
  file_attrs attributes()
  {
    return handle_.attributes();
  }

  /// @copydoc file_handle::attributes(error_code&)
  ASIOEXT_WINDOWS_NO_HANDLEINFO_WARNING
  file_attrs attributes(error_code& ec) ASIOEXT_NOEXCEPT
  {
    return handle_.attributes(ec);
  }

  /// @copydoc file_handle::attributes(file_attrs,file_attr_options)
  ASIOEXT_WINDOWS_NO_HANDLEINFO_WARNING
  void attributes(file_attrs attrs, file_attr_options opts =
                    file_attr_options::replace)
  {
    handle_.attributes(attrs, opts);
  }

  /// @copydoc file_handle::attributes(file_attrs,file_attr_options,error_code&)
  ASIOEXT_WINDOWS_NO_HANDLEINFO_WARNING
  void attributes(file_attrs attrs, file_attr_options opts,
                  error_code& ec) ASIOEXT_NOEXCEPT
  {
    handle_.attributes(attrs, opts, ec);
  }

  /// @copydoc file_handle::attributes(file_attrs,error_code&)
  ASIOEXT_WINDOWS_NO_HANDLEINFO_WARNING
  void attributes(file_attrs attrs, error_code& ec) ASIOEXT_NOEXCEPT
  {
    handle_.attributes(attrs, ec);
  }

  /// @copydoc file_handle::times()
  file_times times()
  {
    return handle_.times();
  }

  /// @copydoc file_handle::times(error_code&)
  file_times times(error_code& ec) ASIOEXT_NOEXCEPT
  {
    return handle_.times(ec);
  }

  /// @copydoc file_handle::times(const file_times&)
  void times(const file_times& new_times)
  {
    handle_.times(new_times);
  }

  /// @copydoc file_handle::times(const file_times&,error_code&)
  void times(const file_times& new_times, error_code& ec) ASIOEXT_NOEXCEPT
  {
    handle_.times(new_times, ec);
  }

  /// @}

  /// @name SyncReadStream functions
  /// @{

  /// @copydoc file_handle::read_some(const MutableBufferSequence&)
  template <typename MutableBufferSequence>
  std::size_t read_some(const MutableBufferSequence& buffers)
  {
    return handle_.read_some(buffers);
  }

  /// @copydoc file_handle::read_some(const MutableBufferSequence&,error_code&)
  template <typename MutableBufferSequence>
  std::size_t read_some(const MutableBufferSequence& buffers, error_code& ec)
  {
    return handle_.read_some(buffers, ec);
  }

  /// @}

  /// @name SyncWriteStream functions
  /// @{

  /// @copydoc file_handle::write_some(const ConstBufferSequence&)
  template <typename ConstBufferSequence>
  std::size_t write_some(const ConstBufferSequence& buffers)
  {
    return handle_.write_some(buffers);
  }

  /// @copydoc file_handle::write_some(const ConstBufferSequence&,error_code&)
  template <typename ConstBufferSequence>
  std::size_t write_some(const ConstBufferSequence& buffers, error_code& ec)
  {
    return handle_.write_some(buffers, ec);
  }

  /// @}

  /// @name SyncRandomAccessReadDevice functions
  /// @{

  /// @copydoc file_handle::read_some_at(uint64_t,const MutableBufferSequence&)
  template <typename MutableBufferSequence>
  std::size_t read_some_at(uint64_t offset,
                           const MutableBufferSequence& buffers)
  {
    return handle_.read_some_at(offset, buffers);
  }

  /// @copydoc file_handle::read_some_at(uint64_t,const MutableBufferSequence&,error_code&)
  template <typename MutableBufferSequence>
  std::size_t read_some_at(uint64_t offset,
                           const MutableBufferSequence& buffers,
                           error_code& ec)
  {
    return handle_.read_some_at(offset, buffers, ec);
  }

  /// @}

  /// @name SyncRandomAccessWriteDevice functions
  /// @{

  /// @copydoc file_handle::write_some_at(uint64_t,const ConstBufferSequence&)
  template <typename ConstBufferSequence>
  std::size_t write_some_at(uint64_t offset, const ConstBufferSequence& buffers)
  {
    return handle_.write_some_at(offset, buffers);
  }

  /// @copydoc file_handle::write_some_at(uint64_t,const ConstBufferSequence&,error_code&)
  template <typename ConstBufferSequence>
  std::size_t write_some_at(uint64_t offset,
                            const ConstBufferSequence& buffers,
                            error_code& ec)
  {
    return handle_.write_some_at(offset, buffers, ec);
  }

  /// @}

private:
  // Prevent copying
  unique_file_handle(const unique_file_handle&) ASIOEXT_DELETED;
  unique_file_handle& operator=(const unique_file_handle&) ASIOEXT_DELETED;

  file_handle handle_;
};

ASIOEXT_NS_END

#if defined(ASIOEXT_HEADER_ONLY)
# include "asioext/impl/unique_file_handle.cpp"
#endif

#endif
