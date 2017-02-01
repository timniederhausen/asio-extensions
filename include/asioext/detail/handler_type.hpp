/// @copyright Copyright (c) 2017 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_DETAIL_HANDLERTYPE_HPP
#define ASIOEXT_DETAIL_HANDLERTYPE_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#if defined(ASIOEXT_USE_BOOST_ASIO)
# include <boost/asio/handler_type.hpp>
# include <boost/asio/detail/handler_type_requirements.hpp>
#else
# include <asio/handler_type.hpp>
# include <asio/detail/handler_type_requirements.hpp>
#endif

#if defined(ASIOEXT_USE_BOOST_ASIO)
# define ASIOEXT_HANDLER_TYPE BOOST_ASIO_HANDLER_TYPE
# define ASIOEXT_COMPLETION_HANDLER_CHECK BOOST_ASIO_COMPLETION_HANDLER_CHECK
# define ASIOEXT_READ_HANDLER_CHECK BOOST_ASIO_READ_HANDLER_CHECK
# define ASIOEXT_WRITE_HANDLER_CHECK BOOST_ASIO_WRITE_HANDLER_CHECK
#else
# define ASIOEXT_HANDLER_TYPE ASIO_HANDLER_TYPE
# define ASIOEXT_COMPLETION_HANDLER_CHECK ASIO_COMPLETION_HANDLER_CHECK
# define ASIOEXT_READ_HANDLER_CHECK ASIO_READ_HANDLER_CHECK
# define ASIOEXT_WRITE_HANDLER_CHECK ASIO_WRITE_HANDLER_CHECK
#endif

ASIOEXT_NS_BEGIN

// ASIO_HANDLER_TYPE doesn't prefix types with the asio:: namespace.
using asio::handler_type;

ASIOEXT_NS_END

#endif
