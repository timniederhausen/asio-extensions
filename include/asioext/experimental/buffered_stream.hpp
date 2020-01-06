/// @file
/// Defines the buffered_stream class
///
/// @copyright Copyright (c) 2019 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_EXPERIMENTAL_BUFFEREDSTREAM_HPP
#define ASIOEXT_EXPERIMENTAL_BUFFEREDSTREAM_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include "asioext/linear_buffer.hpp"
#include "asioext/error_code.hpp"
#include "asioext/async_result.hpp"
#include "asioext/compose.hpp"
#include "asioext/bind_handler.hpp"

#include "asioext/detail/throw_error.hpp"

#if defined(ASIOEXT_USE_BOOST_ASIO)
# include <boost/asio/associated_executor.hpp>
# include <boost/asio/read.hpp>
# include <boost/asio/write.hpp>
#else
# include <asio/associated_executor.hpp>
# include <asio/read.hpp>
# include <asio/write.hpp>
#endif

#include <type_traits>
#include <utility>

ASIOEXT_NS_BEGIN

template <typename Stream, typename Allocator = std::allocator<uint8_t>>
class buffered_stream
{
public:
  /// The type of the next layer.
  typedef typename std::remove_reference<Stream>::type next_layer_type;

  /// The type of the lowest layer.
  typedef typename next_layer_type::lowest_layer_type lowest_layer_type;

  /// The type of the executor associated with the object.
  typedef typename lowest_layer_type::executor_type executor_type;

  typedef asioext::basic_linear_buffer<Allocator> buffer_type;

  /// Construct, passing the specified argument to initialise the next layer.
  template <typename Arg>
  explicit buffered_stream(
      Arg&& a, std::size_t max_write_size = (std::numeric_limits<std::size_t>::max)(),
      std::size_t max_read_size = (std::numeric_limits<std::size_t>::max)())
    : stream_(std::forward<Arg>(a))
    , write_buffer_(0, max_write_size)
    , write_buffer_locked_(0, max_write_size)
    , read_buffer_(0, max_read_size)
  {}

  /// Construct, passing the specified argument to initialise the next layer.
  template <typename Arg>
  buffered_stream(
      Arg&& a, const Allocator& allocator,
      std::size_t max_write_size = (std::numeric_limits<std::size_t>::max)(),
      std::size_t max_read_size = (std::numeric_limits<std::size_t>::max)())
    : stream_(std::forward<Arg>(a))
    , write_buffer_(allocator, 0, max_write_size)
    , write_buffer_locked_(allocator, 0, max_write_size)
    , read_buffer_(allocator, 0, max_read_size)
  {}

  /// Get a reference to the next layer.
  next_layer_type& next_layer() ASIOEXT_NOEXCEPT
  {
    return stream_;
  }

  /// Get a reference to the lowest layer.
  lowest_layer_type& lowest_layer() ASIOEXT_NOEXCEPT
  {
    return stream_.lowest_layer();
  }

  /// Get a const reference to the lowest layer.
  const lowest_layer_type& lowest_layer() const ASIOEXT_NOEXCEPT
  {
    return stream_.lowest_layer();
  }

  /// Get the executor associated with the object.
  executor_type get_executor() ASIOEXT_NOEXCEPT
  {
    return stream_.lowest_layer().get_executor();
  }

  /// Close the stream.
  void close()
  {
    write_buffer_.clear();
    write_buffer_locked_.clear();
    read_buffer_.clear();
    stream_.close();
  }

  /// Close the stream.
  void close(error_code& ec) ASIOEXT_NOEXCEPT
  {
    write_buffer_.clear();
    write_buffer_locked_.clear();
    read_buffer_.clear();
    stream_.close(ec);
  }

  buffer_type& write_buffer() ASIOEXT_NOEXCEPT
  {
    return write_buffer_;
  }

  const buffer_type& write_buffer() const ASIOEXT_NOEXCEPT
  {
    return write_buffer_;
  }

  bool can_flush() const ASIOEXT_NOEXCEPT
  {
    return !flush_pending_ && !write_buffer_.empty();
  }

  bool flush_pending() const ASIOEXT_NOEXCEPT { return flush_pending_; }

  /// @brief Flush all data from the buffer to the next layer.
  ///
  /// This function tries to write all buffered data to the next layer.
  /// The function call will block until one or more bytes of data has been
  /// written successfully, or until an error occurs.
  ///
  /// @return the number of bytes written to the next layer on the last write
  /// operation.
  ///
  /// @throws asio::system_error Thrown on failure.
  ///
  /// @note The flush operation may not write all of the internal buffer.
  std::size_t flush()
  {
    error_code ec;
    const std::size_t s = flush(ec);
    detail::throw_error(ec, "flush");
    return s;
  }

  /// @brief Flush all data from the buffer to the next layer.
  ///
  /// This function tries to write all buffered data to the next layer.
  /// The function call will block until one or more bytes of data has been
  /// written successfully, or until an error occurs.
  ///
  /// @return the number of bytes written to the next layer on the last write
  /// operation.
  ///
  /// @param ec Set to indicate what error occurred. If no error occurred,
  /// the object is reset.
  ///
  /// @note The flush operation may not write all of the internal buffer.
  std::size_t flush(error_code& ec);

  /// @brief Asynchronously flush all data from the buffer to the next layer.
  ///
  /// This function is used to asynchronously write all buffered data to the
  /// next layer.
  ///
  /// @param handler The handler to be called when the flush operation completes.
  /// The function signature of the handler must be:
  /// @code void handler(
  ///   const error_code& error, // Result of operation.
  ///   std::size_t bytes_transferred // Number of bytes written.
  /// ); @endcode
  /// Regardless of whether the asynchronous operation completes immediately or
  /// not, the handler will not be invoked from within this function. Invocation
  /// of the handler will be performed in a manner equivalent to using
  /// asio::io_context::post().
  ///
  /// @note The flush operation may not write all of the internal buffer.
  ///
  /// @warning Starting multiple asynchronous flushes is not allowed.
  template <typename WriteHandler>
  ASIOEXT_INITFN_RESULT_TYPE(WriteHandler, void (error_code, std::size_t))
  async_flush(WriteHandler&& handler);

  buffer_type& read_buffer() ASIOEXT_NOEXCEPT
  {
    return read_buffer_;
  }

  const buffer_type& read_buffer() const ASIOEXT_NOEXCEPT
  {
    return read_buffer_;
  }

  /// @brief Fill the buffer with some data.
  ///
  /// This function tries to read data from the next layer.
  /// The function call will block until one or more bytes of data has been
  /// read successfully, or until an error occurs.
  ///
  /// @return the number of bytes placed in the buffer as a result of
  /// the operation.
  ///
  /// @throws asio::system_error Thrown on failure.
  std::size_t fill()
  {
    error_code ec;
    const std::size_t s = fill(ec);
    detail::throw_error(ec, "fill");
    return s;
  }

  /// @brief Fill the buffer with some data.
  ///
  /// This function tries to read data from the next layer.
  /// The function call will block until one or more bytes of data has been
  /// read successfully, or until an error occurs.
  ///
  /// @return the number of bytes placed in the buffer as a result of
  /// the operation.
  ///
  /// @param ec Set to indicate what error occurred. If no error occurred,
  /// the object is reset.
  std::size_t fill(error_code& ec);

  /// @brief Asynchronously fill the buffer with some data.
  ///
  /// This function tries to asynchronously read data from the next layer.
  ///
  /// @param handler The handler to be called when the fill operation completes.
  /// The function signature of the handler must be:
  /// @code void handler(
  ///   const error_code& error, // Result of operation.
  ///   std::size_t bytes_transferred // Number of bytes read.
  /// ); @endcode
  /// Regardless of whether the asynchronous operation completes immediately or
  /// not, the handler will not be invoked from within this function. Invocation
  /// of the handler will be performed in a manner equivalent to using
  /// asio::io_context::post().
  ///
  /// @warning Starting multiple asynchronous fills is not allowed.
  template <typename ReadHandler>
  ASIOEXT_INITFN_RESULT_TYPE(ReadHandler, void (error_code, std::size_t))
  async_fill(ReadHandler&& handler);

private:
  buffer_type* lock_write_buffers(error_code& ec);
  void unlock_write_buffers(std::size_t bytes_written);

  Stream stream_;

  bool flush_pending_ = false;
  buffer_type write_buffer_;
  buffer_type write_buffer_locked_;
  buffer_type read_buffer_;
};

template <typename Stream, typename Allocator>
std::size_t buffered_stream<Stream, Allocator>::flush(error_code& ec)
{
  const auto buffer = lock_write_buffers(ec);
  if (ec) return 0;

  const std::size_t r = asio::write(stream_, asio::dynamic_buffer(*buffer),
                                    ec);
  unlock_write_buffers(r);
  return r;
}

template <typename Stream, typename Allocator>
template <typename WriteHandler>
ASIOEXT_INITFN_RESULT_TYPE(WriteHandler, void(error_code, std::size_t))
buffered_stream<Stream, Allocator>::async_flush(WriteHandler&& handler)
{
  auto init = [this] (auto&& handler) {
    error_code ec;
    const auto buffer = lock_write_buffers(ec);
    if (ec) {
      auto ex = asio::get_associated_executor(handler, get_executor());
      asio::post(ex, asioext::bind_handler(std::move(handler), ec, 0));
      return;
    }

    auto op = [this] (auto& self, error_code ec, std::size_t size) {
      unlock_write_buffers(size);
      self.complete(ec, size);
    };
    asio::async_write(stream_, asio::dynamic_buffer(*buffer),
                      asioext::make_composed_operation(std::move(op),
                                                       std::move(handler)));
  };
  return asioext::async_initiate<WriteHandler, void (error_code, std::size_t)>(
    init, handler);
}

template <typename Stream, typename Allocator>
std::size_t buffered_stream<Stream, Allocator>::fill(error_code& ec)
{
  return asio::read(stream_, asio::dynamic_buffer(read_buffer_),
                    asio::transfer_at_least(1), ec);
}

template <typename Stream, typename Allocator>
template <typename ReadHandler>
ASIOEXT_INITFN_RESULT_TYPE(ReadHandler, void(error_code, std::size_t))
buffered_stream<Stream, Allocator>::async_fill(ReadHandler&& handler)
{
  return asio::async_read(stream_, asio::dynamic_buffer(read_buffer_),
                          asio::transfer_at_least(1),
                          std::forward<ReadHandler>(handler));
}

template <typename Stream, typename Allocator>
typename buffered_stream<Stream, Allocator>::buffer_type*
buffered_stream<Stream, Allocator>::lock_write_buffers(boost::system::error_code& ec)
{
  if (flush_pending_) {
    ec = asio::error::already_started;
    return nullptr;
  }

  if (write_buffer_locked_.size() == 0) {
    // Don't swap if there's nothing in either buffer (we'll do a zero-sized
    // write in this case, which doesn't hurt us and simplifies the call-site).
    if (write_buffer_.size() != 0) {
      // Swap send buffers so clients can continue to enqueue data while
      // this buffer is being flushed.
      std::swap(write_buffer_, write_buffer_locked_);
    }
  }

  flush_pending_ = true;
  return &write_buffer_locked_;
}

template <typename Stream, typename Allocator>
void buffered_stream<Stream, Allocator>::unlock_write_buffers(std::size_t bytes_written)
{
  flush_pending_ = false;
}

ASIOEXT_NS_END

#endif
