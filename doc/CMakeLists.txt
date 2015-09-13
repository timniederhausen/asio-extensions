# Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

find_package(Doxygen)

if (NOT DOXYGEN_FOUND)
	message(WARNING "Doxygen was not found, |doc| target is disabled")
	return()
endif()

configure_file(doxyfile.in doxyfile @ONLY)

add_custom_target(asioext.doc ALL
	COMMAND ${DOXYGEN_EXECUTABLE} doxyfile
	VERBATIM
)
