/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_ERRORCODE_HPP
#define ASIOEXT_ERRORCODE_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#if defined(ASIOEXT_USE_BOOST_ASIO)
# include <boost/system/error_code.hpp>
#else
# if !defined(ASIO_HAS_STD_SYSTEM_ERROR)
#  error Missing standard library error codes (<system_error>)
# endif
# include <system_error>
#endif

ASIOEXT_NS_BEGIN

#if defined(ASIOEXT_IS_DOCUMENTATION)
/// @ingroup compat
/// @defgroup error Error handling
/// @{

/// @brief Typedef for the error_code class used by this library.
///
/// If Boost.Asio is used (i.e. @ref ASIOEXT_USE_BOOST_ASIO is defined),
/// this will be a typedef for @c boost::system::error_code.
/// Otherwise this'll be a typedef for @c std::error_code.
typedef automatically_chosen error_code;

/// Either @c boost::system::error_category or @c std::error_category.
typedef automatically_chosen error_category;

/// Either @c boost::system::system_category or @c std::system_category.
typedef automatically_chosen system_category;

/// Either @c boost::system::generic_category or @c std::generic_category.
typedef automatically_chosen generic_category;

/// Either @c boost::system::errc or @c std::errc.
typedef automatically_chosen errc;

/// @}
#elif defined(ASIOEXT_USE_BOOST_ASIO)
using boost::system::error_code;
using boost::system::error_category;
using boost::system::system_category;
using boost::system::generic_category;
namespace errc = boost::system::errc;
#else
using std::error_code;
using std::error_category;
using std::system_category;
using std::generic_category;
using std::errc;
#endif

// We want error_category objects without dynamic initialization.
// Achieving this is quite tedious, as older versions of Boost and some
// standard library implementations do not mark their error_category
// constructors as `constexpr`.
//
// The following machinery allows us to fall back on dynamically initialized
// error_category's if need be.

// ASIOEXT_ERRORCATEGORY_CONSTEXPR_CTOR: Is std::error_category's ctor constexpr?
#if !defined(ASIOEXT_ERRORCATEGORY_CONSTEXPR_CTOR)
# if defined(ASIOEXT_HAS_CONSTEXPR14)
#  if defined(ASIOEXT_USE_BOOST_ASIO)
#   if (BOOST_VERSION >= 106900)
#    define ASIOEXT_ERRORCATEGORY_CONSTEXPR_CTOR 1
#   endif
#  endif
# endif
#endif

// ASIOEXT_ERRORCATEGORY_IN_HEADER: Do we need to define the error_category
// subclass in the header?
#if !defined(ASIOEXT_ERRORCATEGORY_IN_HEADER) && !defined(ASIOEXT_IS_DOCUMENTATION)
# if defined(ASIOEXT_ERRORCATEGORY_CONSTEXPR_CTOR)
#  define ASIOEXT_ERRORCATEGORY_IN_HEADER 1
# elif defined(ASIOEXT_HEADER_ONLY) || defined(ASIOEXT_SOURCE)
#  define ASIOEXT_ERRORCATEGORY_IN_HEADER 1
# endif
#endif

#if defined(ASIOEXT_ERRORCATEGORY_CONSTEXPR_CTOR) && !defined(ASIOEXT_IS_DOCUMENTATION)
// Use `extern template` if we're not header-only
# if !defined(ASIOEXT_HEADER_ONLY)
#  if defined(ASIOEXT_SOURCE)
#   define ASIOEXT_DECLARE_ERRORCATEGORY_HOLDER(TYPE, NAME) \
      template struct detail::NAME ## _holder<void>;
#  else
#   define ASIOEXT_DECLARE_ERRORCATEGORY_HOLDER(TYPE, NAME) \
      extern template struct detail::NAME ## _holder<void>;
#  endif
# else
#  define ASIOEXT_DECLARE_ERRORCATEGORY_HOLDER(TYPE, NAME)
# endif

# define ASIOEXT_DECLARE_ERRORCATEGORY(TYPE, NAME) \
  constexpr const error_category& NAME() ASIOEXT_NOEXCEPT; \
  namespace detail { \
  template <typename T> struct NAME ## _holder { \
    static constexpr TYPE instance{}; \
  }; \
  template<typename T> constexpr TYPE NAME ## _holder<T>::instance; } \
  ASIOEXT_DECLARE_ERRORCATEGORY_HOLDER(TYPE, NAME) \
  constexpr const error_category& NAME() ASIOEXT_NOEXCEPT \
  { return detail::NAME ## _holder<void>::instance; }
# define ASIOEXT_DEFINE_ERRORCATEGORY(TYPE, NAME)
#else
# define ASIOEXT_DECLARE_ERRORCATEGORY(TYPE, NAME) \
  ASIOEXT_DECL const error_category& NAME() ASIOEXT_NOEXCEPT;
# define ASIOEXT_DEFINE_ERRORCATEGORY(TYPE, NAME) \
  const error_category& NAME() ASIOEXT_NOEXCEPT \
  { static const TYPE instance; return instance; }
#endif

ASIOEXT_NS_END

#endif
