# Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

if (ASIOEXT_STANDALONE)
	add_subdirectory(cpp11)
endif ()

if (ASIOEXT_USE_BOOST_ASIO)
	add_subdirectory(cpp03)
endif ()
