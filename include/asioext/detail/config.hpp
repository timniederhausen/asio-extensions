/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_DETAIL_CONFIG_HPP
#define ASIOEXT_DETAIL_CONFIG_HPP

#if defined(ASIOEXT_STANDALONE)
# define ASIOEXT_DISABLE_BOOST_FILESYSTEM 1
#else
# include <boost/config.hpp>
# include <boost/version.hpp>
# define ASIOEXT_HAS_BOOST_CONFIG 1
#endif

#if defined(ASIOEXT_USE_BOOST_ASIO)
# include <boost/asio/detail/config.hpp>
#else
# include <asio/detail/config.hpp>
#endif

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
// Define as |inline| so multiple copies of our functions are discarded.
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

// ASIOEXT_MSVC: Macro expanding to the MSVC version (i.e. the _MSC_VER value).
#if !defined(ASIOEXT_MSVC)
# if defined(ASIOEXT_HAS_BOOST_CONFIG) && defined(BOOST_MSVC)
#  define ASIOEXT_MSVC BOOST_MSVC
# elif defined(_MSC_VER) && (defined(__INTELLISENSE__) \
      || (!defined(__MWERKS__) && !defined(__EDG_VERSION__)))
#  define ASIOEXT_MSVC _MSC_VER
# endif
#endif

// ASIOEXT_HAS_CLANG_LIBCXX: Defined if Clang is used with libc++.
#if defined(__clang__)
# if (__cplusplus >= 201103)
#  if __has_include(<__config>)
#   include <__config>
#   if defined(_LIBCPP_VERSION)
#    define ASIOEXT_HAS_CLANG_LIBCXX 1
#   endif
#  endif
# endif
#endif

// ASIOEXT_HAS_MOVE: Defined if the compiler supports rvalue references
// (and ASIOEXT_DISABLE_MOVE isn't defined).
#if !defined(ASIOEXT_HAS_MOVE) && !defined(ASIOEXT_DISABLE_MOVE)
# if defined(__clang__)
#  if __has_feature(__cxx_rvalue_references__)
#   define ASIOEXT_HAS_MOVE 1
#  endif
# endif
# if defined(__GNUC__)
#  if ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 5)) || (__GNUC__ > 4)
#   if defined(__GXX_EXPERIMENTAL_CXX0X__)
#    define ASIOEXT_HAS_MOVE 1
#   endif
#  endif
# endif
# if defined(ASIOEXT_MSVC) && (ASIOEXT_MSVC >= 1700)
#  define ASIOEXT_HAS_MOVE 1
# endif
#endif

// ASIOEXT_HAS_VARIADIC_TEMPLATES: Defined if the compiler supports variadic templates
// (and ASIOEXT_DISABLE_VARIADIC_TEMPLATES isn't defined).
#if !defined(ASIOEXT_HAS_VARIADIC_TEMPLATES) && !defined(ASIOEXT_DISABLE_VARIADIC_TEMPLATES)
# if defined(__clang__)
#  if __has_feature(__cxx_variadic_templates__)
#   define ASIOEXT_HAS_VARIADIC_TEMPLATES 1
#  endif
# endif
# if defined(__GNUC__)
#  if ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 3)) || (__GNUC__ > 4)
#   if defined(__GXX_EXPERIMENTAL_CXX0X__)
#    define ASIOEXT_HAS_VARIADIC_TEMPLATES 1
#   endif
#  endif
# endif
# if defined(ASIOEXT_MSVC) && (ASIOEXT_MSVC >= 1800)
#  define ASIOEXT_HAS_VARIADIC_TEMPLATES 1
# endif
#endif

// ASIOEXT_DELETED: Macro expanding to '= delete' on supported compilers.
#if !defined(ASIOEXT_DELETED)
# if defined(__GNUC__)
#  if ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 7)) || (__GNUC__ > 4)
#   if defined(__GXX_EXPERIMENTAL_CXX0X__)
#    define ASIOEXT_DELETED = delete
#   endif
#  endif
# endif
# if defined(__clang__)
#  if __has_feature(__cxx_deleted_functions__)
#   define ASIOEXT_DELETED = delete
#  endif
# endif
# if !defined(ASIOEXT_DELETED)
#  define ASIOEXT_DELETED
# endif
#endif

// ASIOEXT_NOEXCEPT: Macro expanding to 'noexcept' on supported compilers.
#if !defined(ASIOEXT_NOEXCEPT)
# if !defined(ASIOEXT_DISABLE_NOEXCEPT)
#  if (BOOST_VERSION >= 105300)
#   define ASIOEXT_NOEXCEPT BOOST_NOEXCEPT
#  elif defined(__clang__)
#   if __has_feature(__cxx_noexcept__)
#    define ASIOEXT_NOEXCEPT noexcept(true)
#   endif
#  elif defined(__GNUC__)
#   if ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 7)) || (__GNUC__ > 4)
#    if defined(__GXX_EXPERIMENTAL_CXX0X__)
#      define ASIOEXT_NOEXCEPT noexcept(true)
#    endif
#   endif
#  elif defined(ASIOEXT_MSVC) && (ASIOEXT_MSVC >= 1900)
#    define ASIOEXT_NOEXCEPT noexcept(true)
#  endif
# endif
# if !defined(ASIOEXT_NOEXCEPT)
#  define ASIOEXT_NOEXCEPT
# endif
#endif

// ASIOEXT_HAS_STD_TYPE_TRAITS: <type_traits> header
#if !defined(ASIOEXT_HAS_STD_TYPE_TRAITS)
# if !defined(ASIOEXT_DISABLE_STD_TYPE_TRAITS)
#  if defined(__clang__)
#   if (__cplusplus >= 201103)
#    if __has_include(<type_traits>)
#     define ASIOEXT_HAS_STD_TYPE_TRAITS 1
#    endif
#   endif
#  endif
#  if defined(__GNUC__)
#   if ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 5)) || (__GNUC__ > 4)
#    if defined(__GXX_EXPERIMENTAL_CXX0X__)
#     define ASIOEXT_HAS_STD_TYPE_TRAITS 1
#    endif
#   endif
#  endif
#  if defined(ASIOEXT_MSVC) && (ASIOEXT_MSVC >= 1700)
#   define ASIOEXT_HAS_STD_TYPE_TRAITS 1
#  endif
# endif
#endif

// Standard library support for shared_ptr and weak_ptr.
#if !defined(ASIOEXT_HAS_STD_SHARED_PTR)
# if !defined(ASIOEXT_DISABLE_STD_SHARED_PTR)
#  if defined(__clang__)
#   if defined(ASIOEXT_HAS_CLANG_LIBCXX)
#    define ASIOEXT_HAS_STD_SHARED_PTR 1
#   elif (__cplusplus >= 201103)
#    define ASIOEXT_HAS_STD_SHARED_PTR 1
#   endif
#  endif
#  if defined(__GNUC__)
#   if ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 3)) || (__GNUC__ > 4)
#    if defined(__GXX_EXPERIMENTAL_CXX0X__)
#     define ASIOEXT_HAS_STD_SHARED_PTR 1
#    endif
#   endif
#  endif
#  if defined(ASIOEXT_MSVC) && (ASIOEXT_MSVC >= 1600)
#   define ASIOEXT_HAS_STD_SHARED_PTR 1
#  endif
# endif
#endif

// ASIOEXT_HAS_FILE_FLAGS: Defined if the OS has a fchflags syscall
// and the stat struct has a st_flags member.
#if !defined(ASIOEXT_HAS_FILE_FLAGS)
# if !defined(ASIOEXT_DISABLE_FILE_FLAGS)
#  if defined(__FreeBSD__) || defined (__APPLE__) || defined (__OpenBSD__) || \
      defined (__NetBSD__)
#   define ASIOEXT_HAS_FILE_FLAGS 1
#  endif
# endif
#endif

// ASIOEXT_HAS_PRAGMA_ONCE: Defined if the compiler supports '#pragma once'
#if !defined(ASIOEXT_HAS_PRAGMA_ONCE)
# if !defined(ASIOEXT_DISABLE_PRAGMA_ONCE)
#  if defined(__clang__)
#   define ASIOEXT_HAS_PRAGMA_ONCE 1
#  endif
#  if defined(__GNUC__)
#   if ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 4)) || (__GNUC__ > 3)
// GCC supports it, but recent versions(?) seem to have performance
// penalties associated with this: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=58770
//#    define ASIOEXT_HAS_PRAGMA_ONCE 1
#   endif
#  endif
#  if defined(ASIOEXT_MSVC) && (ASIOEXT_MSVC >= 1020)
#   define ASIOEXT_HAS_PRAGMA_ONCE 1
#  endif
# endif
#endif

#if !defined(ASIOEXT_WINDOWS)
# if defined(ASIOEXT_HAS_BOOST_CONFIG) && defined(BOOST_WINDOWS)
#  define ASIOEXT_WINDOWS 1
# elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#  define ASIOEXT_WINDOWS 1
# endif
#endif

#if !defined(ASIOEXT_HAS_BOOST_FILESYSTEM)
# if !defined(ASIOEXT_DISABLE_BOOST_FILESYSTEM)
#  if (BOOST_VERSION >= 104600)
#   define ASIOEXT_HAS_BOOST_FILESYSTEM 1
#  endif
# endif
#endif

// ASIOEXT_USE_BOOST_ASIO: Defines whether the standalone or
// Boost version of Asio is used.
//
// Makes asioext::asio an alias for boost::asio.
#if defined(ASIOEXT_USE_BOOST_ASIO)
namespace boost { namespace asio {} }
ASIOEXT_NS_BEGIN
namespace asio = boost::asio;
ASIOEXT_NS_END
#endif

#endif
