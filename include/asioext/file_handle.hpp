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
#pragma once
#endif

#if defined(ASIOEXT_WINDOWS)
# include "asioext/detail/win_file_ops.hpp"
#else
# include "asioext/detail/posix_file_ops.hpp"
#endif

#include <asio/error_code.hpp>

#if defined(ASIOEXT_USE_BOOST_FILESYSTEM) || defined(ASIOEXT_IS_DOCUMENTATION)
#include <boost/filesystem/path.hpp>
#endif

#include "asioext/detail/push_options.hpp"

ASIOEXT_NS_BEGIN

/// @brief A thin and lightweight wrapper around a native file handle.
///
/// The file_handle class provides uniform access to
/// native OS file handles, wrapping the relevant OS APIs in portable
/// methods.
///
/// file_handle objects cannot be copied, but are move-constructible/
/// move-assignable if a compiler with C++11 support is used.
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
/// @note If copying a handle (which is a costly operation) is really necessary,
/// the @ref duplicate_handle function can be used.
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

  /// Describes the various position offset origins.
  ///
  /// This enum describes the various origins,
  /// relative to which a position offset might be interpreted.
  enum seek_origin
  {
    /// Interpret the offset as relative to the file's beginning
    /// (i.e. absolute)
    from_begin,

    /// Interpret the offset as relative to the current offset
    from_current,

    /// Interpret the offset as relative to the file's size.
    from_end,
  };

  /// @brief Construct an empy file_handle.
  ///
  /// This constructor initializes the file_handle to an empty state.
  ASIOEXT_DECL file_handle() ASIOEXT_NOEXCEPT;

  /// Destroy a file_handle.
  ///
  /// This destructor attempts to close the currently owned file handle.
  /// Failures are silently ignored.
  ASIOEXT_DECL ~file_handle();

  /// @brief Open a file and construct a file_handle.
  ///
  /// This constructor initializes the file_handle with a new handle to a file.
  ///
  /// @param filename The path of the file to open.
  ///
  /// @param flags Flags used to open the file.
  /// For a detailed reference, see @ref open_flags.
  ///
  /// @throws asio::system_error Thrown on failure.
  ///
  /// @see open_flags
  ASIOEXT_DECL file_handle(const std::string& filename,
                           uint_fast32_t flags);

  /// @brief Open a file and construct a file_handle.
  ///
  /// This constructor initializes the file_handle with a new handle to a file.
  ///
  /// @param filename The path of the file to open.
  ///
  /// @param flags Flags used to open the file.
  /// For a detailed reference, see @ref open_flags.
  ///
  /// @param ec Set to indicate what error occurred. If no error occurred,
  /// the object is reset.
  ///
  /// @see open_flags
  ASIOEXT_DECL file_handle(const std::string& filename,
                           uint_fast32_t flags,
                           asio::error_code& ec) ASIOEXT_NOEXCEPT;

#if defined(ASIOEXT_USE_BOOST_FILESYSTEM) || defined(ASIOEXT_IS_DOCUMENTATION)
  /// @copydoc file_handle(const std::string&,uint_fast32_t)
  ASIOEXT_DECL file_handle(const boost::filesystem::path& filename,
                           uint_fast32_t flags);

  /// @copydoc file_handle(const std::string&,uint_fast32_t,asio::error_code&)
  ASIOEXT_DECL file_handle(const boost::filesystem::path& filename,
                           uint_fast32_t flags,
                           asio::error_code& ec) ASIOEXT_NOEXCEPT;
#endif

#ifdef ASIOEXT_HAS_MOVE
  /// @brief Move-construct a file_handle from another.
  ///
  /// This constructor moves a handle from one object to another.
  ///
  /// @param other The other file_handle object from which the move will occur.
  ///
  /// @note Following the move, the moved-from object is in the same state as if
  /// constructed using the @c file_handle() constructor.
  ASIOEXT_DECL file_handle(file_handle&& other) ASIOEXT_NOEXCEPT;

  /// @brief Move-assign a file_handle from another.
  ///
  /// This assignment operator moves a handle from one object to another.
  ///
  /// @param other The other file_handle object from which the move will occur.
  ///
  /// @note Following the move, the moved-from object is in the same state as if
  /// constructed using the @c file_handle() constructor.
  ASIOEXT_DECL file_handle& operator=(file_handle&& other);
#endif

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
  native_handle_type native_handle() ASIOEXT_NOEXCEPT
  {
    return handle_;
  }

  /// @name Handle-management functions
  /// @{

  /// @brief Open a file and assign its handle to this file_handle.
  ///
  /// This function opens the handle to hold a new handle to a file.
  /// Any previously held handle is closed.
  ///
  /// @param filename The path of the file to open.
  ///
  /// @param flags Flags used to open the file.
  /// For a detailed reference, see @ref open_flags.
  ///
  /// @throws asio::system_error Thrown on failure.
  ///
  /// @see open_flags
  ASIOEXT_DECL void open(const std::string& filename,
                         uint_fast32_t flags);

  /// @brief Open a file and assign its handle to this file_handle.
  ///
  /// This function opens the handle to hold a new handle to a file.
  /// Any previously held handle is closed.
  ///
  /// @param filename The path of the file to open.
  ///
  /// @param flags Flags used to open the file.
  /// For a detailed reference, see @ref open_flags.
  ///
  /// @param ec Set to indicate what error occurred. If no error occurred,
  /// the object is reset.
  ///
  /// @see open_flags
  ASIOEXT_DECL void open(const std::string& filename,
                         uint_fast32_t flags,
                         asio::error_code& ec) ASIOEXT_NOEXCEPT;

#if defined(ASIOEXT_USE_BOOST_FILESYSTEM) || defined(ASIOEXT_IS_DOCUMENTATION)
  /// @copydoc open(const std::string&,uint_fast32_t)
  ASIOEXT_DECL void open(const boost::filesystem::path& filename,
                         uint_fast32_t flags);

  /// @copydoc open(const std::string&,uint_fast32_t,asio::error_code&)
  ASIOEXT_DECL void open(const boost::filesystem::path& filename,
                         uint_fast32_t flags,
                         asio::error_code& ec) ASIOEXT_NOEXCEPT;
#endif

  /// Determine whether the handle is open.
  ASIOEXT_DECL bool is_open() const ASIOEXT_NOEXCEPT;

  /// @brief Close the handle.
  ///
  /// This function is used to close the handle.
  ///
  /// @throws asio::system_error Thrown on failure.
  ASIOEXT_DECL void close();

  /// @brief Close the handle.
  ///
  /// This function is used to close the handle.
  ///
  /// @param ec Set to indicate what error occurred. If no error occurred,
  /// the object is reset.
  ASIOEXT_DECL void close(asio::error_code& ec) ASIOEXT_NOEXCEPT;

  /// @brief Assign an existing native handle to the handle.
  ///
  /// This function opens the handle to hold an existing native handle.
  /// Any previously held handle is closed.
  ///
  /// @param handle A native handle.
  ///
  /// @throws asio::system_error Thrown on failure.
  ASIOEXT_DECL void assign(const native_handle_type& handle);

  /// @brief Assign an existing native handle to the file_handle.
  ///
  /// This function opens the handle to hold an existing native handle.
  /// Any previously held handle will be closed.
  ///
  /// @param handle A native handle.
  ///
  /// @param ec Set to indicate what error occurred. If no error occurred,
  /// the object is reset.
  ASIOEXT_DECL void assign(const native_handle_type& handle,
                           asio::error_code& ec) ASIOEXT_NOEXCEPT;

  /// @}

  /// @name Size/positioning functions
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
  ASIOEXT_DECL uint64_t size(asio::error_code& ec) ASIOEXT_NOEXCEPT;

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
  ASIOEXT_DECL uint64_t position(asio::error_code& ec);

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
  ASIOEXT_DECL uint64_t seek(seek_origin origin, int64_t offset,
                             asio::error_code& ec);

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
  /// To read into a single data buffer use the @c asio::buffer function as follows:
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
                        asio::error_code& ec);

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
  /// peer. Consider using the @c asio::write function if you need to ensure that
  /// all data is written before the blocking operation completes.
  template <typename ConstBufferSequence>
  std::size_t write_some(const ConstBufferSequence& buffers,
                         asio::error_code& ec);

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
  /// To read into a single data buffer use the @c asio::buffer function as follows:
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
                           asio::error_code& ec);

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
                            asio::error_code& ec);

  /// @}

private:
  // Prevent copying
  file_handle(const file_handle&) ASIOEXT_DELETED;
  file_handle& operator=(const file_handle&) ASIOEXT_DELETED;

  native_handle_type handle_;
};

/// @brief Duplicate the given file_handle.
///
/// This function duplicates @c handle and returns a new @c file_handle
/// object. The new handle refers to the same file as @c handle,
/// but doesn't necessarily have the same native handle.
///
/// @return A new file_handle referring to the same file as @c handle.
///
/// @note This is provided as a free function instead of a e.g. copy-contructor
/// since copying file_handles is a non-trivial operation which always needs to
/// duplicate the underlying OS handle and thus isn't what you would expect
/// when copying a lightweight handle object.
file_handle duplicate_handle(const file_handle& handle);

ASIOEXT_NS_END

#include "asioext/detail/pop_options.hpp"

#include "asioext/impl/file_handle_win.hpp"

#if defined(ASIOEXT_HEADER_ONLY)
# if defined(ASIOEXT_WINDOWS)
#  include "asioext/impl/file_handle_win.cpp"
# else
#  include "asioext/impl/file_handle_posix.cpp"
# endif
#endif

#endif
