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

/// @brief Assume that AsioExt has been built in a seperate CU.
///
/// A separate compilation unit must
/// <code>\#include &lt;asioext/impl/src.cpp&gt;</code>.
#define ASIOEXT_SEPARATE_COMPILATION

/// @brief Assume that AsioExt has been built as a shared library.
///
/// On Windows, AsioExt will declare functions with
/// <code>__declspec(dllimport)</code>.
#define ASIOEXT_DYN_LINK

/// @brief On Windows assume <code>const char*</code> filenames
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

/// @brief Forcefully disable C++11 rvalue reference support.
///
/// Disable the use of rvalue references, regardless of
/// compiler support.
#define ASIOEXT_DISABLE_MOVE

/// @brief Forcefully disable C++11 variadic template support.
///
/// Disable the use of variadic templates, regardless of
/// compiler support.
#define ASIOEXT_DISABLE_VARIADIC_TEMPLATES

/// @brief Forcefully disable C++11 @c noexcept support.
///
/// Disable the use of @c noexcept, regardless of
/// compiler support.
#define ASIOEXT_DISABLE_NOEXCEPT

/// @brief Forcefully disable <code>\#pragma once</code> support.
///
/// Disable the use of <code>\#pragma once</code>, regardless of
/// compiler support.
#define ASIOEXT_DISABLE_PRAGMA_ONCE

/// @}
