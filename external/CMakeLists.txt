# Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

# TODO: find_package for both Boost and standalone Asio?
if(NOT ASIOEXT_STANDALONE)
  FetchContent_Declare(
    Boost
    URL https://github.com/boostorg/boost/releases/download/boost-1.81.0/boost-1.81.0.tar.xz
    URL_MD5 6cf0cdd797bca685910d527ae3c08cb3
    DOWNLOAD_EXTRACT_TIMESTAMP ON
    DOWNLOAD_DIR ${CMAKE_SOURCE_DIR}/build-downloads/boost
  )

  set(BOOST_INCLUDE_LIBRARIES asio system filesystem)
  if(ASIOEXT_BUILD_TESTS)
    list(APPEND BOOST_INCLUDE_LIBRARIES test)
  endif()

  FetchContent_MakeAvailable(Boost)
endif()

if(NOT ASIOEXT_USE_BOOST_ASIO AND NOT TARGET asio::header-only)
  add_library(asio-headers INTERFACE)
  add_library(asio::header-only ALIAS asio-headers)
  target_compile_features(asio-headers INTERFACE cxx_std_17)
  target_link_libraries(asio-headers INTERFACE Threads::Threads)

  if ("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
    # Required for clock_gettime() (glibc < 2.17)
    target_link_libraries(asio-headers INTERFACE rt)
  endif ()

  target_include_directories(asio-headers INTERFACE ${ASIOEXT_ASIO_ROOT}/include)
endif()
