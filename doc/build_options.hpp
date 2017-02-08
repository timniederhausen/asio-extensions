/// @file
/// Documents various build options for AsioExt.
///
/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/// @defgroup buildopts Build Options
/// Defining one of the following macros changes AsioExt behaviour.
/// By default, none of them are defined.
///
/// @{

/// @brief Use Boost.Asio instead of standalone Asio.
///
/// If defined, AsioExt will use Boost.Asio headers and types.
#define ASIOEXT_USE_BOOST_ASIO

/// @brief Build/Use AsioExt in a seperate compilation unit.
///
/// This flag **must** be defined when building and consuming  the library.
///
/// Additionally, one compilation unit must
/// <code>\#include &lt;asioext/impl/src.cpp&gt;</code>.
#define ASIOEXT_SEPARATE_COMPILATION

/// @brief Build/Use AsioExt as a shared library.
///
/// This flag **must** be defined when building and consuming the library.
///
/// Additionally, the shared library must have one CU that contains the
/// following line:
/// <code>\#include &lt;asioext/impl/src.cpp&gt;</code>.
///
/// @note On Windows, AsioExt will declare functions with
/// <code>__declspec(dllimport/dllexport)</code>.
#define ASIOEXT_DYN_LINK

/// @brief On Windows, assume <code>const char*</code> filenames
/// are UTF-8 encoded.
///
/// This macro defines whether <tt>const char*</tt> filenames are encoded
/// in the system ANSI code page (if unset) or UTF-8 (if set).
#define ASIOEXT_WINDOWS_USE_UTF8_FILENAMES

/// @brief Disable the use of Boost.Filesystem
///
/// Disable the use of Boost.Filesystem, even if Boost is used
/// and the used version has a compatible Boost.Filesystem version.
#define ASIOEXT_DISABLE_BOOST_FILESYSTEM

/// @brief Disable C++11 rvalue reference support.
///
/// Disable the use of rvalue references, regardless of
/// compiler support.
#define ASIOEXT_DISABLE_MOVE

/// @brief Disable C++11 variadic template support.
///
/// Disable the use of variadic templates, regardless of
/// compiler support.
#define ASIOEXT_DISABLE_VARIADIC_TEMPLATES

/// @brief Disable C++11 @c noexcept support.
///
/// Disable the use of @c noexcept, regardless of
/// compiler support.
#define ASIOEXT_DISABLE_NOEXCEPT

/// @brief Disable <code>\#pragma once</code> support.
///
/// Disable the use of <code>\#pragma once</code>, regardless of
/// compiler support.
#define ASIOEXT_DISABLE_PRAGMA_ONCE

/// @}
