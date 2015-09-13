# Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

# cp example tool
add_executable(asioext.ex.cp cp.cpp)
set_property(TARGET asioext.ex.cp PROPERTY OUTPUT_NAME cp)
target_link_libraries(asioext.ex.cp asioext.static)
