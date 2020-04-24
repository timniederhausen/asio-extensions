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
#include "asioext/detail/buffer.hpp"
#include "asioext/detail/cstdint.hpp"

#include <memory>
#include <limits>
#include <type_traits>

ASIOEXT_NS_BEGIN

/// @ingroup core
/// @brief Basic container-like wrapper around a dynamic size byte array.
///
/// This class templates manages a contiguously-stored array of bytes,
/// allocating memory using the given allocator as needed. Individual bytes
/// are accessible as `uint8_t` values.
template <typename Allocator = std::allocator<uint8_t>>
class basic_linear_buffer
{
public:
  typedef Allocator allocator_type;
  typedef std::allocator_traits<allocator_type> allocator_traits_type;

  typedef uint8_t value_type;
  typedef std::size_t size_type;

  /// The type used to represent an iterator for the buffer's data.
  typedef uint8_t* iterator;

  /// The type used to represent an iterator for a constant
  /// view of the buffer's data.
  typedef const uint8_t* const_iterator;

  /// The type used to represent a reference to a single byte inside the buffer.
  typedef uint8_t& reference;

  /// The type used to represent a const. reference to a single byte inside
  /// the buffer.
  typedef const uint8_t& const_reference;

  /// The type used to represent a const object as a list of buffers.
  typedef asio::const_buffers_1 const_buffers_type;

  /// The type used to represent a non-const object as a list of buffers.
  typedef asio::mutable_buffers_1 mutable_buffers_type;

  static_assert(std::is_same<typename allocator_type::value_type, uint8_t>::value,
                "Allocator::value_type must be uint8_t");

  /// @brief Default-construct a basic_linear_buffer.
  ///
  /// The constructed basic_linear_buffer is empty and doesn't have
  /// any allocated memory.
  basic_linear_buffer() ASIOEXT_NOEXCEPT
    : rep_()
    , capacity_(0)
    , size_(0)
    , max_size_(allocator_traits_type::max_size(rep_))
  {
  }

  /// @brief Construct a linear buffer from an allocator.
  ///
  /// The constructed basic_linear_buffer is empty and doesn't have
  /// any allocated memory.
  ///
  /// @param a The allocator that shall be used to allocate the buffer's storage.
  explicit basic_linear_buffer(const Allocator& a) ASIOEXT_NOEXCEPT
    : rep_(a)
    , capacity_(0)
    , size_(0)
    , max_size_(allocator_traits_type::max_size(rep_))
  {
  }

  /// @brief Construct a linear buffer.
  ///
  /// @param initial_size The initial size that the buffer starts with.
  /// @param maximum_size Specifies a maximum size for the buffer, in bytes.
  basic_linear_buffer(std::size_t initial_size,
                        std::size_t maximum_size =
                            (std::numeric_limits<std::size_t>::max)())
    : rep_()
    , capacity_(initial_size)
    , size_(initial_size)
    , max_size_((std::min)(allocator_traits_type::max_size(rep_),
                           maximum_size))
  {
    rep_.data_ = allocator_traits_type::allocate(rep_, initial_size);
  }

  /// @brief Construct a linear buffer from an allocator.
  ///
  /// @param a The allocator that shall be used to allocate the buffer's storage.
  /// @param initial_size The initial size that the buffer starts with.
  /// @param maximum_size Specifies a maximum size for the buffer, in bytes.
  basic_linear_buffer(const Allocator& a, std::size_t initial_size,
                        std::size_t maximum_size =
                            (std::numeric_limits<std::size_t>::max)())
    : rep_(a)
    , capacity_(initial_size)
    , size_(initial_size)
    , max_size_((std::min)(allocator_traits_type::max_size(rep_),
                           maximum_size))
  {
    rep_.data_ = allocator_traits_type::allocate(rep_, initial_size);
  }

  /// @brief Copy-construct a linear buffer.
  basic_linear_buffer(const basic_linear_buffer& other);

  /// @brief Move-construct a linear buffer.
  ///
  /// After the move, @c other is an empty buffer with no allocated memory
  /// (as-if just default-constructed).
  basic_linear_buffer(basic_linear_buffer&& other) ASIOEXT_NOEXCEPT
    : rep_(std::move(other.rep_))
    , capacity_(other.capacity_)
    , size_(other.size_)
    , max_size_(other.max_size_)
  {
    other.capacity_ = other.size_ = 0;
  }

  /// @brief Destroy the basic_linear_buffer.
  ///
  /// Deallocates all owned data.
  ~basic_linear_buffer()
  {
    if (rep_.data_)
      allocator_traits_type::deallocate(rep_, rep_.data_, capacity_);
  }

  /// @brief Copy-assign a linear buffer.
  basic_linear_buffer& operator=(const basic_linear_buffer& other);

  /// @brief Move-assign a linear buffer.
  ///
  /// After the move, @c other is an empty buffer with no allocated memory
  /// (as-if just default-constructed).
  basic_linear_buffer& operator=(basic_linear_buffer&& other) ASIOEXT_NOEXCEPT;

  /// @brief Get the size of the input sequence.
  std::size_t size() const ASIOEXT_NOEXCEPT
  {
    return size_;
  }

  /// @brief Get the maximum size of the linear buffer.
  ///
  /// @returns The allowed maximum of the sum of the sizes of the input sequence
  /// and output sequence.
  std::size_t max_size() const ASIOEXT_NOEXCEPT
  {
    return max_size_;
  }

  /// @brief Get the current capacity of the linear buffer.
  ///
  /// @returns The current total capacity of the buffer, i.e. for both the input
  /// sequence and output sequence.
  std::size_t capacity() const ASIOEXT_NOEXCEPT
  {
    return capacity_;
  }

  /// @brief Determine if this linear buffer is empty.
  bool empty() const ASIOEXT_NOEXCEPT
  {
    return 0 == size_;
  }

  /// @brief Get an iterator pointing at the buffer data beginning.
  iterator begin() ASIOEXT_NOEXCEPT { return rep_.data_; }

  /// @brief Get an iterator pointing at the buffer data end.
  iterator end() ASIOEXT_NOEXCEPT { return rep_.data_ + size_; }

  /// @brief Get an iterator pointing at the buffer data beginning.
  const_iterator begin() const ASIOEXT_NOEXCEPT
  {
    return rep_.data_;
  }

  /// @brief Get an iterator pointing at the buffer data end.
  const_iterator end() const ASIOEXT_NOEXCEPT
  {
    return rep_.data_ + size_;
  }

  /// @brief Get a pointer to the buffer data beginning.
  uint8_t* data() ASIOEXT_NOEXCEPT { return rep_.data_; }

  /// @brief Get a pointer to the buffer data beginning.
  const uint8_t* data() const ASIOEXT_NOEXCEPT { return rep_.data_; }

  /// Get a list of buffers that represents this object's memory.
  ///
  /// @returns An object of type @c mutable_buffers_type that satisfies
  /// MutableBufferSequence requirements, representing the memory in the
  /// input sequence.
  ///
  /// @note The returned object is invalidated by any @c basic_linear_buffer
  /// member function that causes a reallocation.
  mutable_buffers_type as_buffers() ASIOEXT_NOEXCEPT
  {
    return mutable_buffers_type(rep_.data_, size_);
  }

  /// Get a list of buffers that represents this object's memory.
  ///
  /// @returns An object of type @c const_buffers_type that satisfies
  /// ConstBufferSequence requirements, representing the memory in the
  /// input sequence.
  ///
  /// @note The returned object is invalidated by any @c basic_linear_buffer
  /// member function that causes a reallocation.
  const_buffers_type as_buffers() const ASIOEXT_NOEXCEPT
  {
    return const_buffers_type(rep_.data_, size_);
  }

  /// @brief Get a reference to a specific byte inside the buffer.
  ///
  /// @param i Offset of the wanted byte.
  reference operator[](std::size_t i) ASIOEXT_NOEXCEPT
  {
    return rep_.data_[i];
  }

  /// @brief Get a reference to a specific byte inside the buffer.
  ///
  /// @param i Offset of the wanted byte.
  const_reference operator[](std::size_t i) const ASIOEXT_NOEXCEPT
  {
    return rep_.data_[i];
  }

  /// @brief Append the given data to the buffer.
  ///
  /// This function appends the given raw data to the buffer,
  /// resizing it as necessary.
  ///
  /// If the buffer is resized, all iterators and references
  /// (including the `end()` iterator) are invalidated.
  /// Otherwise only the `end()` iterator is invalidated.
  ///
  /// @param data The raw bytes to append.
  /// @param n Number of raw bytes to append.
  ///
  /// @throws std::length_error If <tt>size() + n > max_size()</tt>.
  void append(const void* data, std::size_t n);

  /// @brief Insert the given data before the specified position.
  ///
  /// This function inserts the given raw data before the buffer position
  /// pointed at by @c before_this. If the new buffer size would exceed the
  /// buffer's capacity, it is resized.
  ///
  /// If the buffer is resized, all iterators and references
  /// (including the `end()` iterator) are invalidated.
  /// Otherwise only the iterators `[before_this, end()]` as well
  /// as their corresponding references are invalidated.
  ///
  /// @param before_this Iterator before which the content will be inserted.
  /// before_this may be the `begin()` or the `end()` iterator.
  /// @param data The raw bytes to insert.
  /// @param n Number of raw bytes to insert.
  void insert(const_iterator before_this, const void* data, std::size_t n)
  { insert(before_this - rep_.data_, data, n); }

  /// @brief Insert the given data before the specified position.
  ///
  /// This function inserts the given raw data before the buffer position
  /// pointed at by @c before_this. If the new buffer size would exceed the
  /// buffer's capacity, it is resized.
  ///
  /// If the buffer is resized, all iterators and references
  /// (including the `end()` iterator) are invalidated.
  /// Otherwise only the iterators `[before_this, end()]` as well
  /// as their corresponding references are invalidated.
  ///
  /// @param before_this Position before which the content will be inserted.
  /// before_this may be `0` or the `size()`.
  /// @param data The raw bytes to insert.
  /// @param n Number of raw bytes to insert.
  void insert(std::size_t before_this, const void* data, std::size_t n);

  /// @brief Erase the single byte at the specified position.
  ///
  /// This function erases the single byte at the given position.
  ///
  /// All iterators and references after the erased byte are invalidated.
  ///
  /// @param pos Iterator to the byte to remove. Passing a pseudo-iterator
  /// such as `end()` is not allowed.
  void erase(const_iterator pos)
  { erase(pos - rep_.data_); }

  /// @brief Erase the bytes in the range `[first, last)`.
  ///
  /// This function erases the bytes in the range `[first, last)`.
  ///
  /// All iterators and references after the first erased byte are invalidated.
  ///
  /// @param first Iterator to the first byte to remove.
  /// @param last Iterator to one past the last byte to remove.
  void erase(const_iterator first, const_iterator last)
  { erase(first - rep_.data_, last - rep_.data_); }

  /// @brief Erase the single byte at the specified position.
  ///
  /// This function erases the single byte at the given position.
  ///
  /// All iterators and references after the erased byte are invalidated.
  ///
  /// @param pos Position of the byte to remove. Must be in `[0, size())`.
  void erase(std::size_t pos);

  /// @brief Erase the bytes in the range `[first, last)`.
  ///
  /// This function erases the bytes in the range `[first, last)`.
  ///
  /// All iterators and references after the first erased byte are invalidated.
  ///
  /// @param first Position of the first byte to remove.
  /// @param last Position of one past the last byte to remove.
  void erase(std::size_t first, std::size_t last);

  /// @brief Ensure the buffer has at least the given capacity.
  ///
  /// This function ensures the buffer's capacity is at least @c min_cap.
  ///
  /// If the buffer is reallocated, all iterators and references
  /// (including the `end()` iterator) are invalidated.
  void reserve(std::size_t min_cap);

  /// @brief Resize the buffer.
  ///
  /// This function changes the buffer's size to @c new_size.
  ///
  /// When growing the buffer, new bytes are not zero-initialized
  /// (unlike e.g. <tt>std::vector<uint8_t></tt> which does initialize all
  /// elements).
  ///
  /// If the buffer is resized, all iterators and references
  /// (including the `end()` iterator) are invalidated.
  void resize(std::size_t new_size);

  /// @brief Clear the buffer.
  ///
  /// Resets the buffer to a size of zero without deallocating
  /// the memory.
  void clear() ASIOEXT_NOEXCEPT
  {
    size_ = 0;
  }

private:
  struct representation_type : Allocator
  {
    representation_type() ASIOEXT_NOEXCEPT
      : Allocator()
      , data_(nullptr)
    {}

    explicit representation_type(const Allocator& a) ASIOEXT_NOEXCEPT
      : Allocator(a)
      , data_(nullptr)
    {}

    representation_type(representation_type&& other) ASIOEXT_NOEXCEPT
      : Allocator(std::move(static_cast<allocator_type&>(other)))
      , data_(other.data_)
    {
      other.data_ = nullptr;
    }

    uint8_t* data_;
  };

  void move_assign(basic_linear_buffer& other, std::false_type)
#if defined(ASIOEXT_HAS_ALLOCATOR_ALWAYS_EQUAL)
    ASIOEXT_NOEXCEPT_IF(allocator_traits_type::is_always_equal::value)
#endif
    ;

  void move_assign(basic_linear_buffer& other, std::true_type)
    ASIOEXT_NOEXCEPT_IF(std::is_nothrow_move_assignable<allocator_type>::value);

  template <typename Function>
  void reallocate(std::size_t cap, Function&& cb);

  ASIOEXT_CONSTEXPR std::size_t calculate_capacity(std::size_t n)
      const ASIOEXT_NOEXCEPT
  {
    return capacity_ < max_size_ / 2 ?
        (std::max)(size_ + n, 2 * capacity_) :
        max_size_ - 1;
  }

  representation_type rep_;
  std::size_t capacity_;
  std::size_t size_;
  std::size_t max_size_;
};

template <typename Allocator>
inline asio::const_buffers_1 buffer(const basic_linear_buffer<Allocator>& b)
    ASIOEXT_NOEXCEPT
{
  return asio::buffer(b.data(), b.size());
}

template <typename Allocator>
inline asio::mutable_buffers_1 buffer(basic_linear_buffer<Allocator>& b)
    ASIOEXT_NOEXCEPT
{
  return asio::buffer(b.data(), b.size());
}

/// @brief A linear buffer using the default allocator.
typedef basic_linear_buffer<> linear_buffer;

/// @ingroup core
/// @brief Adapt a @c basic_linear_buffer to the DynamicBuffer requirements.
template <typename Allocator>
class dynamic_linear_buffer
{
public:
  /// The type used to represent the input sequence as a list of buffers.
  typedef asio::const_buffers_1 const_buffers_type;

  /// The type used to represent the output sequence as a list of buffers.
  typedef asio::mutable_buffers_1 mutable_buffers_type;

  /// @brief Construct a dynamic buffer from a @c basic_linear_buffer.
  ///
  /// @param b The basic_linear_buffer to be used as backing storage for
  /// the dynamic buffer.
  /// Any existing data in the buffer is treated as the dynamic buffer's input
  /// sequence. The object stores a reference to the buffer and the user is
  /// responsible for ensuring that the buffer object remains valid until the
  /// dynamic_linear_buffer object is destroyed.
  ///
  /// @param maximum_size Specifies a maximum size for the buffer, in bytes.
  ///
  explicit dynamic_linear_buffer(basic_linear_buffer<Allocator>& b,
      std::size_t maximum_size =
        (std::numeric_limits<std::size_t>::max)()) ASIOEXT_NOEXCEPT
    : data_(b)
#if !defined(ASIOEXT_NO_DYNAMIC_BUFFER_V1)
    , size_((std::numeric_limits<std::size_t>::max)())
#endif
    , max_size_(std::min(b.max_size(), maximum_size))
  {
  }

  /// @brief Move-construct a dynamic buffer.
  ///
  /// The moved-from object should no longer be used afterwards.
  dynamic_linear_buffer(dynamic_linear_buffer&& other) ASIOEXT_NOEXCEPT
    : data_(other.data_)
#if !defined(ASIOEXT_NO_DYNAMIC_BUFFER_V1)
    , size_(other.size_)
#endif
    , max_size_(other.max_size_)
  {
  }

  /// @brief Get the size of the input sequence.
  std::size_t size() const ASIOEXT_NOEXCEPT
  {
#if !defined(ASIOEXT_NO_DYNAMIC_BUFFER_V1)
    if (size_ != (std::numeric_limits<std::size_t>::max)())
      return size_;
#endif
    return (std::min)(data_.size(), max_size_);
  }

  /// @brief Get the maximum size of the dynamic buffer.
  ///
  /// @returns The allowed maximum of the sum of the sizes of the input sequence
  /// and output sequence.
  std::size_t max_size() const ASIOEXT_NOEXCEPT
  {
    return max_size_;
  }

  /// @brief Get the current capacity of the dynamic buffer.
  ///
  /// @returns The current total capacity of the buffer, i.e. for both the input
  /// sequence and output sequence.
  std::size_t capacity() const ASIOEXT_NOEXCEPT
  {
    return (std::min)(data_.capacity(), max_size_);
  }

#if !defined(ASIOEXT_NO_DYNAMIC_BUFFER_V1)
  // XXX: This is an extension anyway
  /// Get a list of buffers that represents the input sequence.
  ///
  /// @returns An object of type @c mutable_buffers_type that satisfies
  /// MutableBufferSequence requirements, representing the memory in the
  /// input sequence.
  ///
  /// @note The returned object is invalidated by any @c dynamic_linear_buffer
  /// member function that modifies the input sequence or output sequence.
  mutable_buffers_type data() ASIOEXT_NOEXCEPT
  {
    return mutable_buffers_type(data_.data(), size_);
  }

  /// Get a list of buffers that represents the input sequence.
  ///
  /// @returns An object of type @c const_buffers_type that satisfies
  /// ConstBufferSequence requirements, representing the memory in the
  /// input sequence.
  ///
  /// @note The returned object is invalidated by any @c dynamic_linear_buffer
  /// member function that modifies the input sequence or output sequence.
  const_buffers_type data() const ASIOEXT_NOEXCEPT
  {
    return const_buffers_type(data_.data(), size_);
  }
#endif

  /// @brief Get a sequence of buffers that represents the underlying memory.
  ///
  /// @param pos Position of the first byte to represent in the buffer sequence
  ///
  /// @param n The number of bytes to return in the buffer sequence. If the
  /// underlying memory is shorter, the buffer sequence represents as many bytes
  /// as are available.
  ///
  /// @returns An object of type @c mutable_buffers_type that satisfies
  /// MutableBufferSequence requirements, representing the linear_buffer memory.
  ///
  /// @note The returned object is invalidated by any @c dynamic_linear_buffer
  /// or @c linear_buffer member function that resizes or erases the linear_buffer.
  ///
  mutable_buffers_type data(std::size_t pos, std::size_t n) ASIOEXT_NOEXCEPT
  {
    const std::size_t siz = data_.size();
    if (pos > siz)
      pos = siz;
    if (pos + n > siz)
      n = siz - pos;
    return mutable_buffers_type(data_.data() + pos, n);
  }

  /// @brief Get a sequence of buffers that represents the underlying memory.
  /**
   * @param pos Position of the first byte to represent in the buffer sequence
   *
   * @param n The number of bytes to return in the buffer sequence. If the
   * underlying memory is shorter, the buffer sequence represents as many bytes
   * as are available.
   *
   * @note The returned object is invalidated by any @c dynamic_linear_buffer
   * or @c linear_buffer member function that resizes or erases the linear_buffer.
   */
  const_buffers_type data(std::size_t pos, std::size_t n) const ASIOEXT_NOEXCEPT
  {
    const std::size_t siz = data_.size();
    if (pos > siz)
      pos = siz;
    if (pos + n > siz)
      n = siz - pos;
    return const_buffers_type(data_.data() + pos, n);
  }

#if !defined(ASIOEXT_NO_DYNAMIC_BUFFER_V1)
  /// @brief Get a list of buffers that represents the output sequence, with
  /// the given size.
  ///
  /// Ensures that the output sequence can accommodate @c n bytes, resizing the
  /// storage object as necessary.
  ///
  /// @param n Total number of bytes the output sequence has to accomodate.
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
  /// @param n Total number of bytes the output sequence has to accomodate.
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
    size_ += (std::min)(n, data_.size() - size_);
    data_.resize(size_);
  }
#endif

  /// @brief Grow the underlying memory by the specified number of bytes.
  ///
  /// Resizes the @c linear_buffer to accommodate an additional @c n bytes
  /// at the end.
  ///
  /// @throws std::length_error If <tt>size() + n > max_size()</tt>.
  ///
  void grow(std::size_t n);

  /// @b Shrink the underlying memory by the specified number of bytes.
  ///
  /// Erases @c n bytes from the end of the vector by resizing the vector
  /// object. If @c n is greater than the current size of the vector, the vector
  /// is emptied.
  ///
  void shrink(std::size_t n)
  {
    const std::size_t siz = size();
    data_.resize(siz - (std::min)(siz, n));
  }

  /// @brief Remove characters from the input sequence.
  ///
  /// Removes @c n characters from the beginning of the input sequence.
  ///
  /// @note If @c n is greater than the size of the input sequence, the entire
  /// input sequence is consumed and no error is issued.
  void consume(std::size_t n)
  {
#if !defined(ASIOEXT_NO_DYNAMIC_BUFFER_V1)
    if (size_ != (std::numeric_limits<std::size_t>::max)()) {
      const std::size_t consume_length = (std::min)(n, size_);
      data_.erase(0, consume_length);
      size_ -= consume_length;
      return;
    }
#endif
    data_.erase(0, (std::min)((std::min)(data_.size(), max_size_), n));
  }

private:
  basic_linear_buffer<Allocator>& data_;
#if !defined(ASIOEXT_NO_DYNAMIC_BUFFER_V1)
  std::size_t size_;
#endif
  std::size_t max_size_;
};

/// @ingroup core
/// @brief Create a new dynamic buffer that represents the
/// given @c basic_linear_buffer.
///
/// @returns <tt>dynamic_linear_buffer<Allocator>(data)</tt>.
template <typename Allocator>
inline dynamic_linear_buffer<Allocator> dynamic_buffer(
    basic_linear_buffer<Allocator>& data) ASIOEXT_NOEXCEPT
{
  return dynamic_linear_buffer<Allocator>(data);
}

ASIOEXT_NS_END

#if !defined(ASIOEXT_IS_DOCUMENTATION)
# if defined(ASIOEXT_USE_BOOST_ASIO)
namespace boost {
# endif
namespace asio {

using asioext::buffer;
using asioext::dynamic_buffer;

}
# if defined(ASIOEXT_USE_BOOST_ASIO)
}
# endif
#endif

#include "asioext/impl/linear_buffer.hpp"

#endif
