# Copyright (c) 2017 Tim Niederhausen (tim@rnc-ag.de)
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

add_executable(asioext.ex.cpp11.simple simple.cpp)
set_property(TARGET asioext.ex.cpp11.simple PROPERTY OUTPUT_NAME simple)
set_property(TARGET asioext.ex.cpp11.simple PROPERTY CXX_STANDARD 11)
target_link_libraries(asioext.ex.cpp11.simple asioext.static)

add_executable(asioext.ex.cpp11.cp cp.cpp)
set_property(TARGET asioext.ex.cpp11.cp PROPERTY OUTPUT_NAME cp)
set_property(TARGET asioext.ex.cpp11.cp PROPERTY CXX_STANDARD 11)
target_link_libraries(asioext.ex.cpp11.cp asioext.static)

add_executable(asioext.ex.cpp11.tee tee.cpp)
set_property(TARGET asioext.ex.cpp11.tee PROPERTY OUTPUT_NAME tee)
set_property(TARGET asioext.ex.cpp11.tee PROPERTY CXX_STANDARD 11)
target_link_libraries(asioext.ex.cpp11.tee asioext.static)
