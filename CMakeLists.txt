# Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.22)
project(asioext VERSION 0.4.0 LANGUAGES CXX)

# Determine if AsioExt is built as a subproject (using add_subdirectory) or if it is the root project.
if(NOT DEFINED ASIOEXT_ROOT_PROJECT)
  set(ASIOEXT_ROOT_PROJECT OFF)
  if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(ASIOEXT_ROOT_PROJECT ON)
  endif()
endif()

include(CMakeDependentOption)
include(FetchContent)

option(ASIOEXT_BUILD_DOC "Build documentation" ${ASIOEXT_ROOT_PROJECT})
option(ASIOEXT_STANDALONE "Only depend on Asio" ON)
option(ASIOEXT_HEADER_ONLY "Don't build any code, just setup a target" OFF)
option(ASIOEXT_BUILD_SHARED "Build the shared library as well" OFF)
option(ASIOEXT_WINDOWS_XP "Build with Windows XP support" OFF)
cmake_dependent_option(ASIOEXT_WINDOWS_USE_UTF8_FILENAMES
                       "Assume char* filenames are UTF-8" OFF
                       "WIN32" OFF)
cmake_dependent_option(ASIOEXT_USE_BOOST_ASIO
                       "Use Boost.Asio instead of standalone Asio" OFF
                       "NOT ASIOEXT_STANDALONE" OFF)
cmake_dependent_option(ASIOEXT_BUILD_TESTS "Build tests" ${ASIOEXT_ROOT_PROJECT}
                       "NOT ASIOEXT_STANDALONE" OFF)
option(ASIOEXT_BUILD_EXAMPLES "Build examples" ${ASIOEXT_ROOT_PROJECT})

find_package(Threads REQUIRED)

if(ASIOEXT_ROOT_PROJECT)
  add_subdirectory(external EXCLUDE_FROM_ALL)
endif()

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
