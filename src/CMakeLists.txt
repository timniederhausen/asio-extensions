# Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

# Header-only target for asioext
add_library(asioext-headers INTERFACE)
target_compile_features(asioext-headers INTERFACE cxx_std_20)
target_include_directories(asioext-headers INTERFACE ${asioext_SOURCE_DIR}/include)

# TODO: only do this if we're the main project!
if(WIN32)
  if(WINDOWS_STORE OR NOT ASIOEXT_WINDOWS_XP)
    target_compile_definitions(asioext-headers INTERFACE _WIN32_WINNT=0x0A00)
  else ()
    target_compile_definitions(asioext-headers INTERFACE _WIN32_WINNT=0x0501)
  endif ()
endif ()

if (ASIOEXT_USE_BOOST_ASIO)
  target_compile_definitions(asioext-headers INTERFACE ASIOEXT_USE_BOOST_ASIO)
  target_link_libraries(asioext-headers INTERFACE Boost::asio Boost::system)
else ()
  target_link_libraries(asioext-headers INTERFACE asio::header-only)
endif ()

if (ASIOEXT_STANDALONE)
  target_compile_definitions(asioext-headers INTERFACE ASIOEXT_STANDALONE)
else()
  target_link_libraries(asioext-headers INTERFACE Boost::filesystem)
endif ()

if (ASIOEXT_WINDOWS_USE_UTF8_FILENAMES)
  # TODO: not strictly necessary, this could be impl. only!
  target_compile_definitions(asioext-headers INTERFACE ASIOEXT_WINDOWS_USE_UTF8_FILENAMES)
endif ()

# Static/shared library target for asioext
add_library(asioext-lib asioext.cpp)
target_compile_definitions(asioext-lib PUBLIC ASIOEXT_SEPARATE_COMPILATION)
target_link_libraries(asioext-lib PUBLIC asioext-headers)

if (BUILD_SHARED_LIBS)
  target_compile_definitions(asioext PUBLIC ASIOEXT_DYN_LINK)
endif ()

# Add aliases for the old target names, now referring to either
# the header-only or the compiled version.
if (ASIOEXT_HEADER_ONLY)
  add_library(asioext::asioext ALIAS asioext-headers)
else ()
  add_library(asioext::asioext ALIAS asioext-lib)
endif ()
