/// @file
/// Defines the basic_linear_buffer class template.
///
/// @copyright Copyright (c) 2018 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_LINEARBUFFER_HPP
#define ASIOEXT_LINEARBUFFER_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include "asioext/error_code.hpp"
#include "asioext/detail/error.hpp"
#include "asioext/detail/buffer.hpp"
#include "asioext/detail/throw_exception.hpp"
#include "asioext/detail/move_support.hpp"

#include <memory>
#include <limits>
#include <type_traits>

ASIOEXT_NS_BEGIN

/// @brief A dynamically sized contiguously stored buffer
///
/// This class templates provides a dynamic memory buffer
/// which satisfies a subset of the
/// DynamicBufferSequence requirements.
///
/// The main difference is that a @ref DynamicBufferSequence
/// is a move-only type that internally refers to a buffer object owned
/// by another object while this class owns the buffer
///
/// @note Unless you are not interested in the data read/written and
/// the buffer object can be destroyed once the operation finishes,
/// it's not recommended to use this class with the
/// @ref DynamicBufferSequence functions provided by Asio.
template <typename Allocator = std::allocator<char>>
class basic_linear_buffer
{
public:
  typedef Allocator allocator_type;
  typedef std::allocator_traits<allocator_type> allocator_traits_type;

  /// The type used to represent the input sequence as a list of buffers.
  typedef asio::const_buffers_1 const_buffers_type;

  /// The type used to represent the output sequence as a list of buffers.
  typedef asio::mutable_buffers_1 mutable_buffers_type;

  static_assert(std::is_same<typename allocator_type::value_type, char>::value,
                "Allocator::value_type must be char");

  /// @brief Default-construct a basic_linear_buffer.
  ///
  /// The constructed basic_linear_buffer is empty and doesn't have
  /// any allocated memory.
  basic_linear_buffer() ASIOEXT_NOEXCEPT
    : data_()
    , capacity_(0)
    , output_size_(0)
    , input_size_(0)
    , max_size_(allocator_traits_type::max_size(data_))
  {
  }

  /// @brief Construct a dynamic buffer from an allocator.
  ///
  /// The constructed basic_linear_buffer is empty and doesn't have
  /// any allocated memory.
  ///
  /// @param a The allocator that shall be used to allocate the buffer's storage.
  explicit basic_linear_buffer(const Allocator& a) ASIOEXT_NOEXCEPT
    : data_(a)
    , capacity_(0)
    , output_size_(0)
    , input_size_(0)
    , max_size_(allocator_traits_type::max_size(data_))
  {
  }

  /// Construct a dynamic buffer.
  ///
  /// @param initial_size The initial size that the buffer starts with.
  /// @param maximum_size Specifies a maximum size for the buffer, in bytes.
  basic_linear_buffer(std::size_t initial_size,
                        std::size_t maximum_size =
                            (std::numeric_limits<std::size_t>::max)())
    : data_()
    , capacity_(initial_size)
    , output_size_(0)
    , input_size_(0)
    , max_size_((std::min)(allocator_traits_type::max_size(data_),
                           maximum_size))
  {
    data_.buffer_ = allocator_traits_type::allocate(data_, initial_size);
  }

  /// Construct a dynamic buffer from an allocator.
  ///
  /// @param a The allocator that shall be used to allocate the buffer's storage.
  /// @param initial_size The initial size that the buffer starts with.
  /// @param maximum_size Specifies a maximum size for the buffer, in bytes.
  basic_linear_buffer(const Allocator& a, std::size_t initial_size,
                        std::size_t maximum_size =
                            (std::numeric_limits<std::size_t>::max)())
    : data_(a)
    , capacity_(initial_size)
    , output_size_(0)
    , input_size_(0)
    , max_size_((std::min)(allocator_traits_type::max_size(data_),
                           maximum_size))
  {
    data_.buffer_ = allocator_traits_type::allocate(data_, initial_size);
  }

  /// @brief Move-construct a dynamic buffer.
  ///
  /// After the move, @c other is an empty buffer with no allocated memory.
  basic_linear_buffer(basic_linear_buffer&& other) ASIOEXT_NOEXCEPT
    : data_(ASIO_MOVE_CAST(allocator_and_buffer)(other.data_))
    , capacity_(other.capacity_)
    , output_size_(other.output_size_)
    , input_size_(other.input_size_)
    , max_size_(other.max_size_)
  {
    other.capacity_ = other.output_size_ = other.input_size_ = 0;
  }

  /// @brief Destroy the basic_linear_buffer.
  ///
  /// Deallocates all owned data.
  ~basic_linear_buffer()
  {
    if (data_.buffer_)
      allocator_traits_type::deallocate(data_, data_.buffer_, capacity_);
  }

  /// @brief Move-assign a dynamic buffer.
  ///
  /// After the move, @c other is an empty buffer with no allocated memory.
  basic_linear_buffer& operator=(basic_linear_buffer&& other) ASIOEXT_NOEXCEPT;

  /// Get the size of the input sequence.
  std::size_t size() const ASIOEXT_NOEXCEPT
  {
    return input_size_;
  }

  /// Get the maximum size of the dynamic buffer.
  ///
  /// @returns The allowed maximum of the sum of the sizes of the input sequence
  /// and output sequence.
  std::size_t max_size() const ASIOEXT_NOEXCEPT
  {
    return max_size_;
  }

  /// Get the current capacity of the dynamic buffer.
  ///
  /// @returns The current total capacity of the buffer, i.e. for both the input
  /// sequence and output sequence.
  std::size_t capacity() const ASIOEXT_NOEXCEPT
  {
    return capacity_;
  }

  /// Get a list of buffers that represents the input sequence.
  ///
  /// @returns An object of type @c const_buffers_type that satisfies
  /// ConstBufferSequence requirements, representing the memory in the
  /// input sequence.
  ///
  /// @note The returned object is invalidated by any @c basic_linear_buffer
  /// member function that modifies the input sequence or output sequence.
  const_buffers_type data() const ASIOEXT_NOEXCEPT
  {
    return const_buffers_type(data_.buffer_, input_size_);
  }

  /// @brief Get a list of buffers that represents the output sequence, with
  /// the given size.
  ///
  /// Ensures that the output sequence can accommodate @c n bytes, resizing the
  /// storage object as necessary.
  ///
  /// @returns An object of type @c mutable_buffers_type that satisfies
  /// MutableBufferSequence requirements, representing memory
  /// at the start of the output sequence of size @c n.
  ///
  /// @throws std::length_error If <tt>size() + n > max_size()</tt>.
  ///
  /// @note The returned object is invalidated by any @c basic_linear_buffer
  /// member function that modifies the input sequence or output sequence.
  mutable_buffers_type prepare(std::size_t n);

  /// @brief Get a list of buffers that represents the output sequence, with
  /// the given size.
  ///
  /// Ensures that the output sequence can accommodate @c n bytes, resizing the
  /// storage object as necessary.
  ///
  /// @param ec Set to indicate what error occurred. If no error occurred,
  /// the object is reset.
  ///
  /// @returns An object of type @c mutable_buffers_type that satisfies
  /// MutableBufferSequence requirements, representing memory
  /// at the start of the output sequence of size @c n.
  ///
  /// @note The returned object is invalidated by any @c basic_linear_buffer
  /// member function that modifies the input sequence or output sequence.
  ///
  /// @note This function is not part of the DynamicBuffer requirements.
  mutable_buffers_type prepare(std::size_t n, error_code& ec);

  /// Move bytes from the output sequence to the input sequence.
  ///
  /// @param n The number of bytes to append from the start of the output
  /// sequence to the end of the input sequence. The remainder of the output
  /// sequence is discarded.
  ///
  /// Requires a preceding call <tt>prepare(x)</tt> where <tt>x >= n</tt>, and
  /// no intervening operations that modify the input or output sequence.
  ///
  /// @note If @c n is greater than the size of the output sequence, the entire
  /// output sequence is moved to the input sequence and no error is issued.
  void commit(std::size_t n)
  {
    input_size_ += (std::min)(n, output_size_);
    output_size_ = 0;
  }

  /// @brief Remove characters from the input sequence.
  ///
  /// Removes @c n characters from the beginning of the input sequence.
  ///
  /// @note If @c n is greater than the size of the input sequence, the entire
  /// input sequence is consumed and no error is issued.
  void consume(std::size_t n)
  {
    const std::size_t consume_length = (std::min)(n, input_size_);
    std::memmove(data_.buffer_, data_.buffer_ + consume_length,
                 input_size_ + output_size_ - consume_length);
    input_size_ -= consume_length;
  }

  /// @brief Clear the buffer.
  ///
  /// Clears the buffer and resets it to zero size, without deallocating
  /// the memory.
  void clear()
  {
    output_size_ = 0;
    input_size_ = 0;
  }

private:
  struct allocator_and_buffer : Allocator
  {
    allocator_and_buffer() ASIOEXT_NOEXCEPT
      : Allocator()
      , buffer_(nullptr)
    {
      // ctor
    }

    explicit allocator_and_buffer(const Allocator& a) ASIOEXT_NOEXCEPT
      : Allocator(a)
      , buffer_(nullptr)
    {
      // ctor
    }

    allocator_and_buffer(allocator_and_buffer&& other) ASIOEXT_NOEXCEPT
      : Allocator(ASIO_MOVE_CAST(allocator_type)(
            static_cast<allocator_type&>(other)))
      , buffer_(other.buffer_)
    {
      other.buffer_ = nullptr;
    }

    char* buffer_;
  };

  void move_assign(basic_linear_buffer& other, std::false_type)
#if defined(ASIOEXT_HAS_ALLOCATOR_ALWAYS_EQUAL)
    ASIOEXT_NOEXCEPT(allocator_traits_type::is_always_equal::value)
#endif
    ;

  void move_assign(basic_linear_buffer& other, std::true_type)
    ASIOEXT_NOEXCEPT(std::is_nothrow_move_assignable<allocator_type>::value);

  void grow_output(std::size_t n);

  allocator_and_buffer data_;
  std::size_t capacity_;
  std::size_t output_size_;
  std::size_t input_size_;
  std::size_t max_size_;
};

template <class Allocator>
basic_linear_buffer<Allocator>& basic_linear_buffer<Allocator>::operator=(
    basic_linear_buffer&& other) ASIOEXT_NOEXCEPT
{
  move_assign(other, std::integral_constant<bool,
      allocator_traits_type::propagate_on_container_move_assignment::value>());

  return *this;
}

template <class Allocator>
typename basic_linear_buffer<Allocator>::mutable_buffers_type
basic_linear_buffer<Allocator>::prepare(std::size_t n)
{
  if (input_size_ > max_size_ || max_size_ - input_size_ < n) {
    std::length_error ex("basic_linear_buffer too long");
    detail::throw_exception(ex);
  }

  if (input_size_ + n > capacity_)
    grow_output(n);
  output_size_ = n;

  return mutable_buffers_type(data_.buffer_ + input_size_, n);
}

template <class Allocator>
typename basic_linear_buffer<Allocator>::mutable_buffers_type
basic_linear_buffer<Allocator>::prepare(std::size_t n, error_code& ec)
{
  if (input_size_ > max_size_ || max_size_ - input_size_ < n) {
    ec = asio::error::no_memory;
    return mutable_buffers_type(nullptr, 0);
  }

  if (input_size_ + n > capacity_)
    grow_output(n);
  output_size_ = n;

  ec = error_code();
  return mutable_buffers_type(data_.buffer_ + input_size_, n);
}

template <class Allocator>
void basic_linear_buffer<Allocator>::move_assign(basic_linear_buffer& other,
                                                 std::false_type)
#if defined(ASIOEXT_HAS_ALLOCATOR_ALWAYS_EQUAL)
  ASIOEXT_NOEXCEPT(allocator_traits_type::is_always_equal::value)
#endif
{
  if (static_cast<allocator_type&>(data_) !=
      static_cast<allocator_type&>(other.data_)) {
    if (data_.buffer_)
      allocator_traits_type::deallocate(data_, data_.buffer_, capacity_);

    capacity_ = other.capacity_;
    output_size_ = other.output_size_;
    input_size_ = other.input_size_;
    data_.buffer_ = allocator_traits_type::allocate(data_, capacity_);
    std::memcpy(data_.buffer_, other.data_.buffer_,
                input_size_ + output_size_);
  } else {
    move_assign(other, std::false_type());
  }
}

template <class Allocator>
void basic_linear_buffer<Allocator>::move_assign(basic_linear_buffer& other,
                                                 std::true_type)
  ASIOEXT_NOEXCEPT(std::is_nothrow_move_assignable<allocator_type>::value)
{
  static_cast<allocator_type&>(data_) =
      std::move(static_cast<allocator_type&>(other.data_));
  data_.buffer_ = other.data_.buffer_;
  capacity_ = other.capacity_;
  output_size_ = other.output_size_;
  input_size_ = other.input_size_;
  max_size_ = other.max_size_;
  other.data_.buffer_ = nullptr;
  other.capacity_ = other.output_size_ = other.input_size_ = 0;
}

template <class Allocator>
void basic_linear_buffer<Allocator>::grow_output(std::size_t n)
{
  const std::size_t cap = capacity_ < max_size_ / 2 ?
      (std::max)(input_size_ + n, 2 * capacity_) :
      max_size_ - 1;

  char* new_buffer = allocator_traits_type::allocate(data_, cap);
  std::memcpy(new_buffer, data_.buffer_, input_size_);
  allocator_traits_type::deallocate(data_, data_.buffer_, capacity_);
  data_.buffer_ = new_buffer;
  capacity_ = cap;
}

/// @brief A linear buffer using the default allocator.
typedef basic_linear_buffer<> linear_buffer;

ASIOEXT_NS_END

#endif
