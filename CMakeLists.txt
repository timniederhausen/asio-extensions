# Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.1)
project(asioext)

include(CMakeDependentOption)

option(ASIOEXT_BUILD_DOC "Build documentation" ON)
option(ASIOEXT_STANDALONE "Only depend on Asio" ON)
option(ASIOEXT_BUILD_SHARED "Build the shared library as well" OFF)
option(ASIOEXT_WINDOWS_XP "Build with Windows XP support" OFF)
cmake_dependent_option(ASIOEXT_WINDOWS_USE_UTF8_FILENAMES
                       "Assume char* filenames are UTF-8" OFF
                       "WIN32" OFF)
cmake_dependent_option(ASIOEXT_USE_BOOST_ASIO
                       "Use Boost.Asio instead of standalone Asio" OFF
                       "NOT ASIOEXT_STANDALONE" OFF)
cmake_dependent_option(ASIOEXT_BUILD_TESTS "Build tests" ON
                       "NOT ASIOEXT_STANDALONE" OFF)
option(ASIOEXT_BUILD_EXAMPLES "Build examples" OFF)

find_package(Threads REQUIRED)

if (NOT ASIOEXT_STANDALONE)
	set(components system filesystem)

	if (ASIOEXT_BUILD_TESTS)
		list(APPEND components unit_test_framework)
	endif ()

	set(Boost_USE_STATIC_LIBS ON)
	find_package(Boost "1.66.0" COMPONENTS ${components} REQUIRED)
endif ()

add_subdirectory(src)

if (ASIOEXT_BUILD_DOC)
	add_subdirectory(doc)
endif ()

if (ASIOEXT_BUILD_EXAMPLES)
	add_subdirectory(example)
endif ()

if (ASIOEXT_BUILD_TESTS)
	add_subdirectory(test)
endif ()
