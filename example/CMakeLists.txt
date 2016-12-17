# Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

# simple example
add_executable(asioext.ex.simple simple.cpp)
set_property(TARGET asioext.ex.simple PROPERTY OUTPUT_NAME simple)
target_link_libraries(asioext.ex.simple asioext.static)

# cp example tool
if (ASIOEXT_STANDALONE)
	add_executable(asioext.ex.cp cp.cpp)
	set_property(TARGET asioext.ex.cp PROPERTY OUTPUT_NAME cp)
	set_property(TARGET asioext.ex.cp PROPERTY CXX_STANDARD 11)
	target_link_libraries(asioext.ex.cp asioext.static)
endif ()

# cp_boost example tool
if (ASIOEXT_USE_BOOST_ASIO)
	add_executable(asioext.ex.cp_boost cp_boost.cpp)
	set_property(TARGET asioext.ex.cp_boost PROPERTY OUTPUT_NAME cp2)
	target_link_libraries(asioext.ex.cp_boost asioext.static)
endif ()
