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
/// @brief Typedef for the error_code class used by this library.
///
/// If Boost.Asio is used (i.e. @ref ASIOEXT_USE_BOOST_ASIO is defined),
/// this will be a typedef for @c boost::system::error_code.
/// Otherwise this'll be a typedef for @c std::error_code.

/// Either @c boost::system::error_code or @c std::error_code.
typedef automatically_chosen error_code;

/// Either @c boost::system::error_category or @c std::error_category.
typedef automatically_chosen error_category;

/// Either @c boost::system::system_category or @c std::system_category.
typedef automatically_chosen system_category;

/// Either @c boost::system::generic_category or @c std::generic_category.
typedef automatically_chosen generic_category;

/// Either @c boost::system::errc or @c std::errc.
typedef automatically_chosen errc;
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

ASIOEXT_NS_END

#endif
