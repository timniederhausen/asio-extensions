# Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.0)
project(asioext)

option(ASIOEXT_BUILD_DOC "Build documentation" ON)
option(ASIOEXT_BUILD_TESTS "Build examples" ON)
option(ASIOEXT_BUILD_EXAMPLES "Build tests" ON)
option(ASIOEXT_STANDALONE "Only depend on Asio" ON)

add_subdirectory(src)

if (ASIOEXT_BUILD_DOC)
	add_subdirectory(doc)
endif ()

if (ASIOEXT_BUILD_TESTS)
	add_subdirectory(example)
endif ()

if (ASIOEXT_BUILD_TESTS)
	add_subdirectory(test)
endif ()
