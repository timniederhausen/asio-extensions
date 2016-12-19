/// @file
/// Defines the scoped_file_handle class
///
/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_SCOPEDFILEHANDLE_HPP
#define ASIOEXT_SCOPEDFILEHANDLE_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include "asioext/detail/error_code.hpp"

#include "asioext/file_handle.hpp"

#if defined(ASIOEXT_HAS_BOOST_FILESYSTEM) || defined(ASIOEXT_IS_DOCUMENTATION)
# include <boost/filesystem/path.hpp>
#endif

ASIOEXT_NS_BEGIN

/// @ingroup files
/// @brief RAII wrapper around file_handle.
///
/// The scoped_file_handle class provides a std::unique_ptr<>-like wrapper
/// around a file_handle. The scoped_file_handle owns the contained native
/// handle.
///
/// scoped_file_handle objects cannot be copied, but are move-constructible/
/// move-assignable if a compiler with C++11 support is used.
///
/// scoped_file_handle models the following asio concepts:
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
/// asioext::scoped_file_handle file("myfile.txt",
///                                  asioext::open_flags::access_write |
///                                  asioext::open_flags::create_always);
///
/// const std::string content = "Hello world";
/// asio::write(file, asio::buffer(content));
/// @endcode
class scoped_file_handle
{
public:
  /// A scoped_file_handle is always the lowest layer.
  typedef scoped_file_handle lowest_layer_type;

  /// @brief Construct an empy scoped_file_handle.
  ///
  /// This constructor initializes the scoped_file_handle to an empty state.
  ASIOEXT_DECL scoped_file_handle() ASIOEXT_NOEXCEPT;

  /// Destroy a scoped_file_handle.
  ///
  /// This destructor attempts to close the currently owned file handle.
  /// Failures are silently ignored.
  ASIOEXT_DECL ~scoped_file_handle();

  /// @brief Construct a scoped_file_handle using a file_handle object.
  ///
  /// This constructor takes ownership of the given file_handle.
  ///
  /// @param handle The file_handle object which shall be assigned to this
  /// scoped_file_handle object.
  ASIOEXT_DECL explicit scoped_file_handle(file_handle handle) ASIOEXT_NOEXCEPT;

  /// @brief Open a file and construct a scoped_file_handle.
  ///
  /// This constructor opens a new handle to the given file.
  ///
  /// @param filename The path of the file to open.
  ///
  /// @param flags Flags used to open the file.
  /// For a detailed reference, see @ref open_flags.
  ///
  /// @throws asio::system_error Thrown on failure.
  ///
  /// @see open_flags
  ASIOEXT_DECL scoped_file_handle(const char* filename, uint32_t flags);

  /// @brief Open a file and construct a scoped_file_handle.
  ///
  /// This constructor opens a new handle to the given file.
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
  ASIOEXT_DECL scoped_file_handle(const char* filename,
                                  uint32_t flags,
                                  error_code& ec) ASIOEXT_NOEXCEPT;

#if defined(ASIOEXT_HAS_BOOST_FILESYSTEM) || defined(ASIOEXT_IS_DOCUMENTATION)
  /// @copydoc scoped_file_handle(const char*,uint32_t)
  ASIOEXT_DECL scoped_file_handle(const boost::filesystem::path& filename,
                                  uint32_t flags);

  /// @copydoc scoped_file_handle(const char*,uint32_t,error_code&)
  ASIOEXT_DECL scoped_file_handle(const boost::filesystem::path& filename,
                                  uint32_t flags,
                                  error_code& ec) ASIOEXT_NOEXCEPT;
#endif

#ifdef ASIOEXT_HAS_MOVE
  /// @brief Move-construct a scoped_file_handle from another.
  ///
  /// This constructor moves a handle from one object to another.
  ///
  /// @param other The other scoped_file_handle object from which the move will
  /// occur.
  ///
  /// @note Following the move, the moved-from object is in the same state as if
  /// constructed using the @c scoped_file_handle() constructor.
  ASIOEXT_DECL scoped_file_handle(scoped_file_handle&& other) ASIOEXT_NOEXCEPT;

  /// @brief Move-assign a scoped_file_handle from another.
  ///
  /// This assignment operator moves a handle from one object to another.
  ///
  /// @param other The other scoped_file_handle object from which the move will
  /// occur.
  ///
  /// @note Following the move, the moved-from object is in the same state as if
  /// constructed using the @c scoped_file_handle() constructor.
  ASIOEXT_DECL scoped_file_handle& operator=(scoped_file_handle&& other);
#endif

  /// @brief Get a reference to the lowest layer.
  ///
  /// This function returns a reference to the lowest layer in a stack of
  /// layers. Since a scoped_file_handle cannot contain any further layers, it
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
  /// layers. Since a scoped_file_handle cannot contain any further layers, it
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

  /// @name Handle-management functions
  /// @{

  /// @brief Open a file and assign its handle to this scoped_file_handle.
  ///
  /// For details, see @ref open(const char*,uint32_t)
  ///
  /// @param filename The path of the file to open.
  ///
  /// @param flags Flags used to open the file.
  /// For a detailed reference, see @ref open_flags.
  ///
  /// @throws asio::system_error Thrown on failure.
  ///
  /// @see open_flags
  ASIOEXT_DECL void open(const char* filename, uint32_t flags);

  /// @brief Open a file and assign its handle to this scoped_file_handle.
  ///
  /// For details, see @ref open(const char*,uint32_t,error_code&)
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
  ASIOEXT_DECL void open(const char* filename,
                         uint32_t flags,
                         error_code& ec) ASIOEXT_NOEXCEPT;

#if defined(ASIOEXT_HAS_BOOST_FILESYSTEM) || defined(ASIOEXT_IS_DOCUMENTATION)
  /// @copydoc open(const char*,uint32_t)
  ///
  /// @note Only available if ASIOEXT_HAS_BOOST_FILESYSTEM is defined.
  ASIOEXT_DECL void open(const boost::filesystem::path& filename,
                         uint32_t flags);

  /// @copydoc open(const char*,uint32_t,error_code&)
  ///
  /// @note Only available if ASIOEXT_HAS_BOOST_FILESYSTEM is defined.
  ASIOEXT_DECL void open(const boost::filesystem::path& filename,
                         uint32_t flags,
                         error_code& ec) ASIOEXT_NOEXCEPT;
#endif

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
  /// the caller. The scoped_file_handle object is reset to an empty state
  /// (i.e. no file opened).
  ///
  /// @return The contained file_handle.
  /// The caller is now responsible to close it.
  ///
  /// @warning This function is dangerous. It is highly unlikely that you'll
  /// ever need to use this.
  ASIOEXT_DECL file_handle leak() ASIOEXT_NOEXCEPT;

  /// @brief Replace the managed file_handle.
  ///
  /// This function closes the currently managed handle (if any),
  /// and replaces it with the given one.
  ///
  /// @param handle The new file_handle to manage.
  /// Ownership is transferred to the scoped_file_handle.
  ///
  /// @throws asio::system_error Thrown on failure.
  ASIOEXT_DECL void reset(file_handle handle);

  /// @brief Replace the managed file_handle.
  ///
  /// This function closes the currently managed handle (if any),
  /// and replaces it with the given one.
  ///
  /// @param handle The new file_handle to manage.
  /// Ownership is transferred to the scoped_file_handle.
  ///
  /// @param ec Set to indicate what error occurred. If no error occurred,
  /// the object is reset.
  ASIOEXT_DECL void reset(file_handle handle, error_code& ec) ASIOEXT_NOEXCEPT;

  /// @}

  /// @name Size/positioning functions
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
  uint64_t seek(file_handle::seek_origin origin, int64_t offset)
  {
    return handle_.seek(origin, offset);
  }

  /// @copydoc file_handle::seek(seek_origin,int64_t,error_code&)
  uint64_t seek(file_handle::seek_origin origin,
                int64_t offset,
                error_code& ec) ASIOEXT_NOEXCEPT
  {
    return handle_.seek(origin, offset, ec);
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
  scoped_file_handle(const scoped_file_handle&) ASIOEXT_DELETED;
  scoped_file_handle& operator=(const scoped_file_handle&) ASIOEXT_DELETED;

  file_handle handle_;
};

ASIOEXT_NS_END

#if defined(ASIOEXT_HEADER_ONLY)
# include "asioext/impl/scoped_file_handle.cpp"
# if defined(ASIOEXT_WINDOWS)
#  include "asioext/impl/scoped_file_handle_win.cpp"
# else
#  include "asioext/impl/scoped_file_handle_posix.cpp"
# endif
#endif

#endif
