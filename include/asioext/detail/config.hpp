/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_DETAIL_CONFIG_HPP
#define ASIOEXT_DETAIL_CONFIG_HPP

#include <asio/detail/config.hpp>

#if ASIOEXT_IS_DOCUMENTATION
# define ASIOEXT_SEPARATE_COMPILATION 1
// When generating the API documentation, we simply assume that all C++
// features are usable.
// Note: Don't define platform-identification macros here!
# define ASIOEXT_HAS_MOVE 1
# define ASIOEXT_HAS_VARIADIC_TEMPLATES 1
# define ASIOEXT_DELETED = delete
# define ASIOEXT_NOEXCEPT noexcept
#endif

#define ASIOEXT_NS_BEGIN namespace asioext {
#define ASIOEXT_NS_END }

// Default to a header-only implementation. The user must specifically request
// separate compilation by defining either ASIOEXT_SEPARATE_COMPILATION or
// ASIOEXT_DYN_LINK (as a DLL/shared library implies separate compilation).
#if !defined(ASIOEXT_HEADER_ONLY)
# if !defined(ASIOEXT_SEPARATE_COMPILATION)
#  if !defined(ASIOEXT_DYN_LINK)
#   define ASIOEXT_HEADER_ONLY 1
#  endif
# endif
#endif

#if defined(ASIOEXT_HEADER_ONLY)
// Define as |inline| so multiple copies of our functions are elided.
# define ASIOEXT_DECL inline
#else
# if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__CODEGEARC__)
#  if defined(ASIOEXT_DYN_LINK)
#   if defined(ASIOEXT_SOURCE)
#    define ASIOEXT_DECL __declspec(dllexport)
#   else
#    define ASIOEXT_DECL __declspec(dllimport)
#   endif
#  endif
# endif
#endif

#if !defined(ASIOEXT_DECL)
# define ASIOEXT_DECL
#endif

#if defined(ASIOEXT_STANDALONE)
# define ASIOEXT_DISABLE_BOOST_FILESYSTEM 1
#endif

#if !defined(ASIOEXT_MSVC) && defined(ASIO_MSVC)
// ASIO_MSVC contains the compiler version (i.e. the _MSC_VER value)
# define ASIOEXT_MSVC ASIO_MSVC
#endif

#if !defined(ASIOEXT_HAS_MOVE) && defined(ASIO_HAS_MOVE)
# define ASIOEXT_HAS_MOVE 1
#endif

#if !defined(ASIOEXT_MOVE_ARG)
# define ASIOEXT_MOVE_ARG ASIO_MOVE_ARG
# define ASIOEXT_MOVE_ARG2 ASIO_MOVE_ARG2
# define ASIOEXT_MOVE_CAST ASIO_MOVE_CAST
# define ASIOEXT_MOVE_CAST2 ASIO_MOVE_CAST2
#endif

#if !defined(ASIOEXT_HAS_VARIADIC_TEMPLATES) && defined(ASIO_HAS_VARIADIC_TEMPLATES)
# define ASIOEXT_HAS_VARIADIC_TEMPLATES 1
#endif

#if !defined(ASIOEXT_DELETED) && defined(ASIO_DELETED)
# define ASIOEXT_DELETED ASIO_DELETED
#endif

#if !defined(ASIOEXT_NOEXCEPT)
# define ASIOEXT_NOEXCEPT ASIO_NOEXCEPT
#endif

// Support for '#pragma once'
#if !defined(ASIOEXT_HAS_PRAGMA_ONCE)
# if !defined(ASIOEXT_DISABLE_PRAGMA_ONCE)
#  if defined(__clang__)
#   define ASIOEXT_HAS_PRAGMA_ONCE 1
#  endif
#  if defined(__GNUC__)
#   if ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 4)) || (__GNUC__ > 3)
// GCC supports it, but recent versions(?) seem to have massive performance
// penalties associated with this: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=58770
//#    define ASIOEXT_HAS_PRAGMA_ONCE 1
#   endif
#  endif
#  if defined(ASIOEXT_MSVC) && (_MSC_VER >= 1020)
#   define ASIOEXT_HAS_PRAGMA_ONCE 1
#  endif
# endif
#endif

#if !defined(ASIOEXT_WINDOWS) && defined(ASIO_WINDOWS)
# define ASIOEXT_WINDOWS 1
#endif

#if !defined(ASIOEXT_HAS_BOOST_FILESYSTEM)
# if !defined(ASIOEXT_DISABLE_BOOST_FILESYSTEM)
#  if (BOOST_VERSION >= 104600)
#   define ASIOEXT_HAS_BOOST_FILESYSTEM 1
#  endif
# endif
#endif

#endif
