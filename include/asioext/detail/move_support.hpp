/// @copyright Copyright (c) 2017 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_DETAIL_MOVESUPPORT_HPP
#define ASIOEXT_DETAIL_MOVESUPPORT_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#if defined(ASIOEXT_HAS_MOVE) && !defined(ASIOEXT_MOVE_CAST)
# define ASIOEXT_MOVE_ARG(type) type&&
# define ASIOEXT_MOVE_CAST(type) static_cast<type&&>
# define ASIOEXT_MOVE_CAST2(type1, type2) static_cast<type1, type2&&>
#endif

#if !defined(ASIOEXT_MOVE_CAST)
# if defined(ASIOEXT_USE_BOOST_ASIO)
# define ASIOEXT_MOVE_ARG(type) BOOST_ASIO_MOVE_ARG(type)
# define ASIOEXT_MOVE_CAST(type) BOOST_ASIO_MOVE_CAST(type)
# define ASIOEXT_MOVE_CAST2(type1, type2) BOOST_ASIO_MOVE_CAST2(type1, type2)
# else
# define ASIOEXT_MOVE_ARG(type) ASIO_MOVE_ARG(type)
# define ASIOEXT_MOVE_CAST(type) ASIO_MOVE_CAST(type)
# define ASIOEXT_MOVE_CAST2(type1, type2) ASIO_MOVE_CAST2(type1, type2)
# endif
#endif

#endif
