/// @file
/// Declares the asioext::cancellation_token utility class.
///
/// @copyright Copyright (c) 2016 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_CANCELLATIONTOKEN_HPP
#define ASIOEXT_CANCELLATIONTOKEN_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include "asioext/detail/memory.hpp"

ASIOEXT_NS_BEGIN

/// @ingroup service
/// @brief Manager for cancellation_tokens.
///
/// This class provides functionality to create and cancel cancellation_tokens.
class cancellation_token_source
{
friend class cancellation_token;

public:
  /// Construct a cancellation_token_source.
  ASIOEXT_DECL cancellation_token_source();

#ifdef ASIOEXT_HAS_MOVE
  /// @brief Move-construct a cancellation_token_source.
  ///
  /// This constructor moves the token source.
  ///
  /// @param other The other cancellation_token_source object from which
  /// the move will occur.
  ///
  /// @note Following the move, the moved-from object is in the same state as if
  /// constructed using the @c cancellation_token_source() constructor.
  ASIOEXT_DECL cancellation_token_source(
      cancellation_token_source&& other) ASIOEXT_NOEXCEPT;

  /// @brief Move-assign a cancellation_token_source.
  ///
  /// This assignment operator moves the token source.
  ///
  /// @param other The other cancellation_token_source object from which
  /// the move will occur.
  ///
  /// @note Following the move, the moved-from object is in the same state as if
  /// constructed using the @c cancellation_token_source() constructor.
  ASIOEXT_DECL cancellation_token_source& operator=(
      cancellation_token_source&& other);
#endif

  /// @brief Cancel all currently issued cancellation_tokens.
  ///
  /// This function cancels all tokens that have been issued
  /// until this call.
  ///
  /// Tokens issued after a call to cancel() are unaffected.
  ASIOEXT_DECL void cancel();

  /// @brief Cancel all tokens, present and future.
  ///
  /// This function cancels all currently issued tokens,
  /// as well as tokens issued after this call.
  ///
  /// All new tokens created from this source will start
  /// in the cancelled state.
  ASIOEXT_DECL void destroy();

  /// @brief Reset a destroyed token source.
  ///
  /// This function resets a destroyed token source,
  /// so that it can be used to issue and cancel tokens again.
  ///
  /// After a call to this function, the object's state is as if
  /// just constructed.
  ASIOEXT_DECL void reset();

private:
  // Prevent copying
  cancellation_token_source(const cancellation_token_source&) ASIOEXT_DELETED;
  cancellation_token_source& operator=(
      const cancellation_token_source&) ASIOEXT_DELETED;

  detail::shared_ptr<void> ptr_;
};

/// @ingroup service
/// @brief Token to determine if an operation was cancelled.
///
/// This class provides the ability to determine if an operation
/// was cancelled. cancellation_token objects can only be created
/// from a cancellation_token_source.
///
/// A cancellation_token is not dependent on the lifetime of its
/// cancellation_token_source (i.e. the token can outlive the source).
class cancellation_token
{
public:
  ASIOEXT_DECL cancellation_token(const cancellation_token_source& source);

  /// @brief Check whether this operation is cancelled.
  ///
  /// This function checks whether this token is cancelled (i.e.
  /// whether cancel() has been called after this token was created).
  ///
  /// @return @c true if the operation was cancelled, @c false otherwise.
  ASIOEXT_DECL bool cancelled() const;

private:
  detail::weak_ptr<void> ptr_;
};

ASIOEXT_NS_END

#if defined(ASIOEXT_HEADER_ONLY)
# include "asioext/impl/cancellation_token.cpp"
#endif

#endif
