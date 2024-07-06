# Copyright (c) 2017 Tim Niederhausen (tim@rnc-ag.de)
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

add_executable(asioext.ex.cpp11.simple simple.cpp)
set_target_properties(asioext.ex.cpp11.simple PROPERTIES OUTPUT_NAME simple CXX_STANDARD 11)
target_link_libraries(asioext.ex.cpp11.simple asioext::asioext)

add_executable(asioext.ex.cpp11.cp cp.cpp)
set_target_properties(asioext.ex.cpp11.cp PROPERTIES OUTPUT_NAME cp CXX_STANDARD 11)
target_link_libraries(asioext.ex.cpp11.cp asioext::asioext)

add_executable(asioext.ex.cpp11.tee tee.cpp)
set_target_properties(asioext.ex.cpp11.tee PROPERTIES OUTPUT_NAME tee CXX_STANDARD 11)
target_link_libraries(asioext.ex.cpp11.tee asioext::asioext)

add_executable(asioext.ex.cpp11.tee_async tee_async.cpp)
set_target_properties(asioext.ex.cpp11.tee_async PROPERTIES OUTPUT_NAME tee_async CXX_STANDARD 11)
target_link_libraries(asioext.ex.cpp11.tee_async asioext::asioext)
