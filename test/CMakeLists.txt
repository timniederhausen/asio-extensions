# Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

add_executable(asioext.test
	scoped_file_handle.cpp
	open_flags.cpp
	main.cpp
)

target_link_libraries(asioext.test asioext.static)
