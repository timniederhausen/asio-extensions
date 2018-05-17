/// @copyright Copyright (c) 2017 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#include "asioext/cancellation_token.hpp"

ASIOEXT_NS_BEGIN

namespace detail {

struct null_deleter
{
  void operator()(void*)
  {
    // I do nothing!
  }
};

}

cancellation_token_source::cancellation_token_source()
  : ptr_(static_cast<void*>(0), detail::null_deleter())
{
  // ctor
}

#ifdef ASIOEXT_HAS_MOVE
cancellation_token_source::cancellation_token_source(
    cancellation_token_source&& other) ASIOEXT_NOEXCEPT
  // TODO(tim): Does this allow us to fulfil our gurantees?
  : ptr_(std::move(other.ptr_))
{
  // ctor
}

cancellation_token_source& cancellation_token_source::operator=(
    cancellation_token_source&& other) ASIOEXT_NOEXCEPT
{
  // TODO(tim): Does this allow us to fulfil our gurantees?
  ptr_ = std::move(other.ptr_);
  return *this;
}
#endif

void cancellation_token_source::cancel()
{
  ptr_.reset(static_cast<void*>(0), detail::null_deleter());
}

void cancellation_token_source::destroy() ASIOEXT_NOEXCEPT
{
  ptr_.reset();
}

void cancellation_token_source::reset()
{
  if (!ptr_)
    ptr_.reset(static_cast<void*>(0), detail::null_deleter());
}

cancellation_token::cancellation_token(
    const cancellation_token_source& token_source) ASIOEXT_NOEXCEPT
  : ptr_(token_source.ptr_)
{
  // ctor
}

bool cancellation_token::cancelled() const ASIOEXT_NOEXCEPT
{
  return ptr_.expired();
}

ASIOEXT_NS_END
