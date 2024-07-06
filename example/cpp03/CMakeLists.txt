# Copyright (c) 2017 Tim Niederhausen (tim@rnc-ag.de)
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

add_executable(asioext.ex.cpp03.cp cp.cpp)
set_property(TARGET asioext.ex.cpp03.cp PROPERTY OUTPUT_NAME cp03)
target_link_libraries(asioext.ex.cpp03.cp asioext::asioext)

add_executable(asioext.ex.cpp03.tee tee.cpp)
set_property(TARGET asioext.ex.cpp03.tee PROPERTY OUTPUT_NAME tee03)
target_link_libraries(asioext.ex.cpp03.tee asioext::asioext)
