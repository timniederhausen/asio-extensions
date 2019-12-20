/// @copyright Copyright (c) 2018 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_IMPL_LINEARBUFFER_HPP
#define ASIOEXT_IMPL_LINEARBUFFER_HPP

#include "asioext/detail/error.hpp"
#include "asioext/detail/throw_exception.hpp"

#include <cstring>

ASIOEXT_NS_BEGIN

template <typename Allocator>
basic_linear_buffer<Allocator>::basic_linear_buffer(
    const basic_linear_buffer& other)
  : rep_()
  , capacity_(other.size_)
  , size_(other.size_)
  , max_size_(other.max_size_)
{
  rep_.data_ = allocator_traits_type::allocate(rep_, other.size_);
  std::memcpy(rep_.data_, other.rep_.data_, size_);
}

template <typename Allocator>
basic_linear_buffer<Allocator>& basic_linear_buffer<Allocator>::operator=(
    const basic_linear_buffer& other)
{
  const size_type n = other.size_;
  if (n > max_size_) {
    std::length_error ex("basic_linear_buffer too long");
    detail::throw_exception(ex);
  }

  if (n > capacity_)
    reallocate(calculate_capacity(n), [this] (uint8_t* new_buffer) {});

  std::memcpy(rep_.data_, other.rep_.data_, n);
  size_ = n;
  return *this;
}

template <typename Allocator>
basic_linear_buffer<Allocator>& basic_linear_buffer<Allocator>::operator=(
    basic_linear_buffer&& other) ASIOEXT_NOEXCEPT
{
  move_assign(other, std::integral_constant<bool,
      allocator_traits_type::propagate_on_container_move_assignment::value>());
  return *this;
}

template <typename Allocator>
void basic_linear_buffer<Allocator>::append(const void* data, std::size_t n)
{
  if (size_ > max_size_ || max_size_ - size_ < n) {
    std::length_error ex("basic_linear_buffer too long");
    detail::throw_exception(ex);
  }

  if (size_ + n > capacity_) {
    reallocate(calculate_capacity(size_ + n), [this] (uint8_t* new_buffer) {
      std::memcpy(new_buffer, rep_.data_, size_);
    });
  }

  std::memcpy(rep_.data_ + size_, data, n);
  size_ += n;
}

template <typename Allocator>
void basic_linear_buffer<Allocator>::insert(std::size_t before_this,
                                            const void* data, std::size_t n)
{
  if (size_ > max_size_ || max_size_ - size_ < n) {
    std::length_error ex("basic_linear_buffer too long");
    detail::throw_exception(ex);
  }

  if (size_ + n > capacity_) {
    reallocate(calculate_capacity(size_ + n),
               [this, before_this, data, n] (uint8_t* new_buffer) {
      std::memcpy(new_buffer, rep_.data_, before_this);
      std::memcpy(new_buffer + before_this, data, n);
      std::memcpy(new_buffer + before_this + n, rep_.data + before_this,
                  size_ - before_this);
    });
  } else {
    std::memmove(rep_.data_ + before_this,
                 rep_.data_ + before_this + n,
                 size_ - before_this);
    std::memcpy(rep_.data_ + before_this, data, n);
  }

  size_ += n;
}

template <typename Allocator>
void basic_linear_buffer<Allocator>::erase(std::size_t pos)
{
  // TODO: assert pos < size
  std::memmove(rep_.data_ + pos, rep_.data_ + pos + 1, size_ - pos - 1);
  --size_;
}

template <typename Allocator>
void basic_linear_buffer<Allocator>::erase(std::size_t first, std::size_t last)
{
  const std::size_t n = last - first;
  std::memmove(rep_.data_ + first, rep_.data_ + last, size_ - first - n);
  size_ -= n;
}

template <typename Allocator>
void basic_linear_buffer<Allocator>::reserve(std::size_t min_cap)
{
  if (min_cap > max_size_) {
    std::length_error ex("basic_linear_buffer too long");
    detail::throw_exception(ex);
  }

  if (min_cap > capacity_) {
    reallocate(min_cap, [this] (uint8_t* new_buffer) {
      std::memcpy(new_buffer, rep_.data_, size_);
    });
  }
}

template <typename Allocator>
void basic_linear_buffer<Allocator>::resize(std::size_t new_size)
{
  if (new_size > max_size_) {
    std::length_error ex("basic_linear_buffer too long");
    detail::throw_exception(ex);
  }

  if (new_size > capacity_) {
    reallocate(calculate_capacity(new_size), [this] (uint8_t* new_buffer) {
      std::memcpy(new_buffer, rep_.data_, size_);
    });
  }

  size_ = new_size;
}

template <typename Allocator>
void basic_linear_buffer<Allocator>::move_assign(basic_linear_buffer& other,
                                                 std::false_type)
#if defined(ASIOEXT_HAS_ALLOCATOR_ALWAYS_EQUAL)
  ASIOEXT_NOEXCEPT_IF(allocator_traits_type::is_always_equal::value)
#endif
{
  if (static_cast<allocator_type&>(rep_) !=
      static_cast<allocator_type&>(other.rep_)) {
    if (rep_.data_)
      allocator_traits_type::deallocate(rep_, rep_.data_, capacity_);

    capacity_ = other.capacity_;
    size_ = other.size_;
    rep_.data_ = allocator_traits_type::allocate(rep_, capacity_);
    std::memcpy(rep_.data_, other.rep_.data_, size_);
  } else {
    move_assign(other, std::false_type());
  }
}

template <typename Allocator>
void basic_linear_buffer<Allocator>::move_assign(basic_linear_buffer& other,
                                                 std::true_type)
  ASIOEXT_NOEXCEPT_IF(std::is_nothrow_move_assignable<allocator_type>::value)
{
  static_cast<allocator_type&>(rep_) =
      std::move(static_cast<allocator_type&>(other.rep_));
  rep_.data_ = other.rep_.data_;
  capacity_ = other.capacity_;
  size_ = other.size_;
  max_size_ = other.max_size_;
  other.rep_.data_ = nullptr;
  other.capacity_ = other.size_ = 0;
}

template <typename Allocator>
template <typename Function>
void basic_linear_buffer<Allocator>::reallocate(std::size_t cap, Function&& fn)
{
  uint8_t* new_buffer = allocator_traits_type::allocate(rep_, cap);
  fn(new_buffer);
  allocator_traits_type::deallocate(rep_, rep_.data_, capacity_);
  rep_.data_ = new_buffer;
  capacity_ = cap;
}

#if !defined(ASIOEXT_NO_DYNAMIC_BUFFER_V1)
template <typename Allocator>
typename dynamic_linear_buffer<Allocator>::mutable_buffers_type
dynamic_linear_buffer<Allocator>::prepare(std::size_t n)
{
  std::size_t siz = size_;
  if (siz == (std::numeric_limits<std::size_t>::max)())
    siz = data_.size();

  if (siz > max_size_ || max_size_ - siz < n) {
    std::length_error ex("dynamic_linear_buffer too long");
    detail::throw_exception(ex);
  }

  size_ = siz;
  data_.resize(siz + n);
  return mutable_buffers_type(data_.data() + siz,
                              data_.size() - siz);
}

template <typename Allocator>
typename dynamic_linear_buffer<Allocator>::mutable_buffers_type
dynamic_linear_buffer<Allocator>::prepare(std::size_t n, error_code& ec)
{
  std::size_t siz = size_;
  if (siz == (std::numeric_limits<std::size_t>::max)())
    siz = data_.size();

  if (siz > max_size_ || max_size_ - siz < n) {
    ec = asio::error::no_memory;
    return mutable_buffers_type(nullptr, 0);
  }

  ec = error_code();

  size_ = siz;
  data_.resize(siz + n);
  return mutable_buffers_type(data_.data() + siz,
                              data_.size() - siz);
}
#endif

template <typename Allocator>
void dynamic_linear_buffer<Allocator>::grow(std::size_t n)
{
  const std::size_t siz = size();
  if (siz > max_size_ || max_size_ - siz < n) {
    std::length_error ex("dynamic_linear_buffer too long");
    detail::throw_exception(ex);
  }
  data_.resize(siz + n);
}

ASIOEXT_NS_END

#endif
