/// @copyright Copyright (c) 2017 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_DETAIL_ASIOVERSION_HPP
#define ASIOEXT_DETAIL_ASIOVERSION_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#if defined(ASIOEXT_USE_BOOST_ASIO)
# include <boost/asio/version.hpp>
# define ASIOEXT_ASIO_VERSION BOOST_ASIO_VERSION
#else
# include <asio/version.hpp>
# define ASIOEXT_ASIO_VERSION ASIO_VERSION
#endif

#endif
