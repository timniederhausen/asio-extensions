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
# include <asio/error_code.hpp>
#endif

ASIOEXT_NS_BEGIN

#if defined(ASIOEXT_IS_DOCUMENTATION)
/// @brief Typedef for the error_code class used by this library.
///
/// If Boost.Asio is used (i.e. @ref ASIOEXT_USE_BOOST_ASIO is defined),
/// this will be a typedef for @c boost::system::error_code.
/// Otherwise this'll be a typedef for asio::error_code
/// (which in turn is a typedef for @c std::error_code if available)
///
/// @note If std::error_code is unavailable, asio::error_code is a bundled
/// custom error_code class, whose interface isn't conforming.
typedef implementation_defined error_code;
#elif defined(ASIOEXT_USE_BOOST_ASIO)
using boost::system::error_code;
#else
using asio::error_code;
#endif

ASIOEXT_NS_END

#endif
