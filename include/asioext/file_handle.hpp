/// @file
/// Defines the file_handle class
///
/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_FILEHANDLE_HPP
#define ASIOEXT_FILEHANDLE_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include "asioext/seek_origin.hpp"
#include "asioext/error_code.hpp"
#include "asioext/chrono.hpp"

#if defined(ASIOEXT_WINDOWS)
# include "asioext/detail/win_file_ops.hpp"
#else
# include "asioext/detail/posix_file_ops.hpp"
#endif

ASIOEXT_NS_BEGIN

/// @ingroup files_time
/// @brief Container for various time points associated with a file.
///
/// This struct contains several time points commonly associated
/// with a file.
/// The availability of the individual time points is platform-dependent.
struct file_times
{
  /// @brief The file's creation time.
  file_time_type ctime;
  /// @brief The file's last access time.
  file_time_type atime;
  /// @brief The file's last modification time.
  file_time_type mtime;
};

inline bool operator==(const file_times& a, const file_times& b)
{ return a.ctime == b.ctime && a.atime == b.atime && a.mtime == b.mtime; }

inline bool operator!=(const file_times& a, const file_times& b)
{ return !(a == b); }

/// @ingroup files_handle
/// @brief A thin and lightweight wrapper around a native file handle.
///
/// The file_handle class provides uniform access to
/// native OS file handles, wrapping the relevant OS APIs in portable
/// methods.
///
/// file_handle doesn't assume ownership of the contained handle.
/// If handle management is desired as well, take a look at the
/// @c unique_file_handle class.
///
/// file_handle models the following asio concepts:
/// * SyncRandomAccessReadDevice
/// * SyncRandomAccessWriteDevice
/// * SyncReadStream
/// * SyncWriteStream
///
/// @par Thread Safety:
/// @e Distinct @e objects: Safe.@n
/// @e Shared @e objects: Unsafe.
///
/// @note This is a low-level component. Most of the time unique_file_handle
/// is a better fit.
class file_handle
{
public:
#if defined(ASIOEXT_IS_DOCUMENTATION)
  /// The operating system's native file handle type.
  typedef implementation_defined native_handle_type;
#elif defined(ASIOEXT_WINDOWS)
  typedef detail::win_file_ops::handle_type native_handle_type;
#else
  typedef detail::posix_file_ops::handle_type native_handle_type;
#endif

  /// A file_handle is always the lowest layer.
  typedef file_handle lowest_layer_type;

  /// @brief Construct an empy file_handle.
  ///
  /// This constructor initializes the file_handle to an empty state.
  ASIOEXT_DECL file_handle() ASIOEXT_NOEXCEPT;

  /// Destroy a file_handle.
  ///
  /// This destructor does nothing. Any contained handle will
  /// <b>not</b> be closed.
  ASIOEXT_DECL ~file_handle();

  /// @brief Construct a file_handle using a native handle.
  ///
  /// This constructor <b>doesn't</b> take ownership of the given native
  /// file handle.
  ///
  /// @param handle The native file handle which shall be assigned to this
  /// file_handle object.
  ASIOEXT_DECL file_handle(const native_handle_type& handle) ASIOEXT_NOEXCEPT;

  /// @brief Get a reference to the lowest layer.
  ///
  /// This function returns a reference to the lowest layer in a stack of
  /// layers. Since a file_handle cannot contain any further layers, it simply
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
  /// layers. Since a file_handle cannot contain any further layers, it simply
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
  native_handle_type native_handle() const ASIOEXT_NOEXCEPT
  {
    return handle_;
  }

  /// @name Handle-management functions
  /// @{

  /// Determine whether the handle is open.
  ASIOEXT_DECL bool is_open() const ASIOEXT_NOEXCEPT;

  /// @brief Close the handle.
  ///
  /// This function closes the contained handle. Does nothing if the object
  /// contains no handle.
  ///
  /// @throws asio::system_error Thrown on failure.
  ASIOEXT_DECL void close();

  /// @brief Close the handle.
  ///
  /// This function closes the contained handle. Does nothing if the object
  /// contains no handle.
  ///
  /// @param ec Set to indicate what error occurred. If no error occurred,
  /// the object is reset.
  ASIOEXT_DECL void close(error_code& ec) ASIOEXT_NOEXCEPT;

  /// @brief Clear the handle.
  ///
  /// This function resets the contained handle to an empty state.
  /// The previous handle <b>is not</b> closed.
  ASIOEXT_DECL void clear() ASIOEXT_NOEXCEPT;

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
  ASIOEXT_DECL uint64_t position();

  /// @brief Get the current file position.
  ///
  /// This function retrieves the current read/write position of this
  /// file handle, relative to the file's beginning.
  ///
  /// @return The current absolute file position.
  ///
  /// @param ec Set to indicate what error occurred. If no error occurred,
  /// the object is reset.
  ASIOEXT_DECL uint64_t position(error_code& ec) ASIOEXT_NOEXCEPT;

  /// @brief Change the read/write position.
  ///
  /// This function repositions the current read/write position of this
  /// file_handle by @c offset, relative to the origin specified
  /// by @c origin.
  ///
  /// @param origin The origin of @c offset.
  ///
  /// @param offset The offset by which the current position's modified.
  ///
  /// @return The new absolute file position.
  ///
  /// @throws asio::system_error Thrown on failure.
  ASIOEXT_DECL uint64_t seek(seek_origin origin, int64_t offset);

  /// @brief Change the read/write position.
  ///
  /// This function repositions the current read/write position of this
  /// file_handle by @c offset, relative to the origin specified
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
  ASIOEXT_DECL uint64_t seek(seek_origin origin,
                             int64_t offset,
                             error_code& ec) ASIOEXT_NOEXCEPT;

  /// @}

  /// @name Metadata functions
  /// @{

  /// @brief Get the size of a file.
  ///
  /// This function retrieves the size of a file, in bytes.
  ///
  /// @return The total number of bytes in this file.
  ///
  /// @throws asio::system_error Thrown on failure.
  ASIOEXT_DECL uint64_t size();

  /// @brief Get the size of a file.
  ///
  /// This function retrieves the size of a file, in bytes.
  ///
  /// @return The total number of bytes in this file.
  ///
  /// @param ec Set to indicate what error occurred. If no error occurred,
  /// the object is reset.
  ASIOEXT_DECL uint64_t size(error_code& ec) ASIOEXT_NOEXCEPT;

  /// @brief Set the size of a file.
  ///
  /// This function resizes the file so its new size matches @c new_size.
  ///
  /// @param new_size The new total size of the file (in bytes).
  ///
  /// @throws asio::system_error Thrown on failure.
  ASIOEXT_DECL void truncate(uint64_t new_size);

  /// @brief Set the size of a file.
  ///
  /// This function resizes the file so its new size matches @c new_size.
  ///
  /// @param new_size The new total size of the file (in bytes).
  ///
  /// @param ec Set to indicate what error occurred. If no error occurred,
  /// the object is reset.
  ASIOEXT_DECL void truncate(uint64_t new_size, error_code& ec) ASIOEXT_NOEXCEPT;

  /// @brief Get the file's current access permissions.
  ///
  /// This function returns the file's current access permissions as
  /// a @c file_perms bitmask.
  ///
  /// @return The file's access permissions.
  ///
  /// @throws asio::system_error Thrown on failure.
  ASIOEXT_WINDOWS_NO_HANDLEINFO_WARNING
  ASIOEXT_DECL file_perms permissions();

  /// @brief Get the file's current access permissions.
  ///
  /// This function returns the file's current access permissions as
  /// a @c file_perms bitmask.
  ///
  /// @return The file's access permissions.
  ///
  /// @param ec Set to indicate what error occurred. If no error occurred,
  /// the object is reset.
  ASIOEXT_WINDOWS_NO_HANDLEINFO_WARNING
  ASIOEXT_DECL file_perms permissions(error_code& ec) ASIOEXT_NOEXCEPT;

  /// @brief Change file access permissions.
  ///
  /// This function changes the file's access permissions.
  /// Depending on whether @c file_perm_options::add_perms,
  /// @c file_perm_options::remove_perms are set,
  /// permissions are either added, removed or replaced.
  ///
  /// @param perms Permissions to set, add or remove.
  ///
  /// @param opts Options controlling this function's
  /// behavior.
  ///
  /// @throws asio::system_error Thrown on failure.
  ASIOEXT_WINDOWS_NO_HANDLEINFO_WARNING
  ASIOEXT_DECL void permissions(file_perms perms,
                                file_perm_options opts =
                                  file_perm_options::replace);

  /// @brief Change file access permissions.
  ///
  /// This function changes the file's access permissions.
  /// Depending on whether @c file_perm_options::add_perms,
  /// @c file_perm_options::remove_perms are set,
  /// permissions are either added, removed or replaced.
  ///
  /// @param perms Permissions to set, add or remove.
  ///
  /// @param ec Set to indicate what error occurred. If no error occurred,
  /// the object is reset.
  ASIOEXT_WINDOWS_NO_HANDLEINFO_WARNING
  ASIOEXT_DECL void permissions(file_perms perms,
                                error_code& ec) ASIOEXT_NOEXCEPT;

  /// @brief Change file access permissions.
  ///
  /// This function changes the file's access permissions.
  /// Depending on whether @c file_perm_options::add_perms,
  /// @c file_perm_options::remove_perms are set,
  /// permissions are either added, removed or replaced.
  ///
  /// @param perms Permissions to set, add or remove.
  ///
  /// @param opts Options controlling this function's
  /// behavior.
  ///
  /// @param ec Set to indicate what error occurred. If no error occurred,
  /// the object is reset.
  ASIOEXT_WINDOWS_NO_HANDLEINFO_WARNING
  ASIOEXT_DECL void permissions(file_perms perms,
                                file_perm_options opts,
                                error_code& ec) ASIOEXT_NOEXCEPT;

  /// @brief Get the file's attributes.
  ///
  /// This function returns the file's attributes as
  /// a @c file_attrs bitmask.
  ///
  /// @return The file's attributes.
  ///
  /// @throws asio::system_error Thrown on failure.
  ASIOEXT_WINDOWS_NO_HANDLEINFO_WARNING
  ASIOEXT_DECL file_attrs attributes();

  /// @brief Get the file's attributes.
  ///
  /// This function returns the file's attributes as
  /// a @c file_attrs bitmask.
  ///
  /// @param ec Set to indicate what error occurred. If no error occurred,
  /// the object is reset.
  ///
  /// @return The file's attributes.
  ASIOEXT_WINDOWS_NO_HANDLEINFO_WARNING
  ASIOEXT_DECL file_attrs attributes(error_code& ec) ASIOEXT_NOEXCEPT;

  /// @brief Change the file's attributes.
  ///
  /// This function changes the file's attributes.
  /// Depending on whether @c file_attr_options::add_attrs,
  /// @c file_attr_options::remove_attrs are set, attributes are either added,
  /// removed or replaced.
  ///
  /// @param attrs Attributes to set, add or remove.
  ///
  /// @param opts Options controlling this function's
  /// behavior.
  ///
  /// @throws asio::system_error Thrown on failure.
  ASIOEXT_WINDOWS_NO_HANDLEINFO_WARNING
  ASIOEXT_DECL void attributes(file_attrs attrs,
                               file_attr_options opts =
                                file_attr_options::replace);

  /// @brief Change the file's attributes.
  ///
  /// This function changes the file's attributes.
  /// Depending on whether @c file_attr_options::add_attrs,
  /// @c file_attr_options::remove_attrs are set, attributes are either added,
  /// removed or replaced.
  ///
  /// @param attrs Attributes to set, add or remove.
  ///
  /// @param ec Set to indicate what error occurred. If no error occurred,
  /// the object is reset.
  ASIOEXT_WINDOWS_NO_HANDLEINFO_WARNING
  ASIOEXT_DECL void attributes(file_attrs attrs,
                               error_code& ec) ASIOEXT_NOEXCEPT;

  /// @brief Change the file's attributes.
  ///
  /// This function changes the file's attributes.
  /// Depending on whether @c file_attr_options::add_attrs,
  /// @c file_attr_options::remove_attrs are set, attributes are either added,
  /// removed or replaced.
  ///
  /// @param attrs Attributes to set, add or remove.
  ///
  /// @param opts Options controlling this function's
  /// behavior.
  ///
  /// @param ec Set to indicate what error occurred. If no error occurred,
  /// the object is reset.
  ASIOEXT_WINDOWS_NO_HANDLEINFO_WARNING
  ASIOEXT_DECL void attributes(file_attrs attrs,
                               file_attr_options opts,
                               error_code& ec) ASIOEXT_NOEXCEPT;

  /// @brief Get the file's time data.
  ///
  /// This function retrieves the various times associated with
  /// a file and copies them into a @c file_times structure.
  ///
  /// Times in the file_times struct that are not available for this
  /// file are zero-initialized.
  ///
  /// @return The file's time data.
  ///
  /// @throws asio::system_error Thrown on failure.
  ASIOEXT_DECL file_times times();

  /// @brief Get the file's time data.
  ///
  /// This function retrieves the various times associated with
  /// a file and copies them into a @c file_times structure.
  ///
  /// Times in the file_times struct that are not available for this
  /// file are zero-initialized.
  ///
  /// @param ec Set to indicate what error occurred. If no error occurred,
  /// the object is reset.
  ///
  /// @return The file's time data.
  ASIOEXT_DECL file_times times(error_code& ec) ASIOEXT_NOEXCEPT;

  /// @brief Change a file's time data.
  ///
  /// This function replaces a file's time data with the given values.
  /// File times that are set to zero inside the @c file_times structure
  /// remain unchanged.
  ///
  /// @param new_times The new file times. Zero values are ignored.
  ///
  /// @throws asio::system_error Thrown on failure.
  ASIOEXT_DECL void times(const file_times& new_times);

  /// @brief Change a file's time data.
  ///
  /// This function replaces a file's time data with the given values.
  /// File times that are set to zero inside the @c file_times structure
  /// remain unchanged.
  ///
  /// @param new_times The new file times. Zero values are ignored.
  ///
  /// @param ec Set to indicate what error occurred. If no error occurred,
  /// the object is reset.
  ASIOEXT_DECL void times(const file_times& new_times,
                          error_code& ec) ASIOEXT_NOEXCEPT;

  /// @}

  /// @name SyncReadStream functions
  /// @{

  /// @brief Read some data from the file.
  ///
  /// This function is used to read data from the file, starting at the
  /// file_handle's current file position. The function call will
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
  std::size_t read_some(const MutableBufferSequence& buffers);

  /// @brief Read some data from the file.
  ///
  /// This function is used to read data from the file, starting at the
  /// file_handle's current file position. The function call will
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
                        error_code& ec) ASIOEXT_NOEXCEPT;

  /// @}

  /// @name SyncWriteStream functions
  /// @{

  /// @brief Write some data to the file.
  ///
  /// This function is used to write data to the file, starting at the
  /// file_handle's current file position. The function call will
  /// block until one or more bytes of the data has been written
  /// successfully, or until an error occurs.
  ///
  /// @param buffers One or more data buffers to be written to the file.
  ///
  /// @returns The number of bytes written.
  ///
  /// @throws asio::system_error Thrown on failure.
  ///
  /// @note The write_some operation may not transmit all of the data to the
  /// peer. Consider using the @c asio::write function if you need to ensure
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
  std::size_t write_some(const ConstBufferSequence& buffers);

  /// @brief Write some data to the file.
  ///
  /// This function is used to write data to the file, starting at the
  /// file_handle's current file position. The function call will
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
  /// @note The write_some operation may not transmit all of the data to the
  /// peer. Consider using the @c asio::write function if you need to ensure
  /// that all data is written before the blocking operation completes.
  template <typename ConstBufferSequence>
  std::size_t write_some(const ConstBufferSequence& buffers,
                         error_code& ec) ASIOEXT_NOEXCEPT;

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
                           const MutableBufferSequence& buffers);

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
                           error_code& ec) ASIOEXT_NOEXCEPT;

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
  std::size_t write_some_at(uint64_t offset,
                            const ConstBufferSequence& buffers);

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
                            error_code& ec) ASIOEXT_NOEXCEPT;

  /// @}

private:
  native_handle_type handle_;
};

ASIOEXT_NS_END

#include "asioext/impl/file_handle.hpp"

#if defined(ASIOEXT_WINDOWS)
# include "asioext/impl/file_handle_win.hpp"
#else
# include "asioext/impl/file_handle_posix.hpp"
#endif

#if defined(ASIOEXT_HEADER_ONLY)
# include "asioext/impl/file_handle.cpp"
# if defined(ASIOEXT_WINDOWS)
#  include "asioext/impl/file_handle_win.cpp"
# else
#  include "asioext/impl/file_handle_posix.cpp"
# endif
#endif

#endif
