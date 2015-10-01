# Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

set(asio_source_files asio.cpp)
set(asioext_source_files asioext.cpp)

add_library(asioext.asio STATIC ${asio_source_files})
set_property(TARGET asioext.asio PROPERTY OUTPUT_NAME asio_static)
target_include_directories(asioext.asio PUBLIC ${ASIO_ROOT}/include)
target_compile_definitions(asioext.asio PUBLIC ASIO_SEPARATE_COMPILATION)
target_link_libraries(asioext.asio Threads::Threads)

if (ASIOEXT_STANDALONE)
	target_compile_definitions(asioext.asio PUBLIC ASIO_STANDALONE)
else ()
	target_include_directories(asioext.asio PUBLIC ${BOOST_ROOT})
	target_compile_definitions(asioext.asio PUBLIC BOOST_ALL_NO_LIB)
	target_link_libraries(asioext.asio ${Boost_LIBRARIES})
endif ()

if (WIN32)
	target_compile_definitions(asioext.asio PUBLIC _WIN32_WINNT=0x0501)
endif ()

add_library(asioext.static STATIC ${asioext_source_files})
set_property(TARGET asioext.static PROPERTY OUTPUT_NAME asioext_static)
target_include_directories(asioext.static PUBLIC ${asioext_SOURCE_DIR}/include)
target_compile_definitions(asioext.static PUBLIC ASIOEXT_SEPARATE_COMPILATION)
target_link_libraries(asioext.static asioext.asio)

if (ASIOEXT_STANDALONE)
	target_compile_definitions(asioext.static PUBLIC ASIOEXT_STANDALONE)
endif ()

add_library(asioext.shared SHARED ${asioext_source_files})
set_property(TARGET asioext.shared PROPERTY OUTPUT_NAME asioext)
target_include_directories(asioext.shared PUBLIC ${asioext_SOURCE_DIR}/include)
target_compile_definitions(asioext.shared PUBLIC ASIOEXT_DYN_LINK PRIVATE PRIVATE ASIOEXT_SOURCE)
target_link_libraries(asioext.shared asioext.asio)

if (ASIOEXT_STANDALONE)
	target_compile_definitions(asioext.shared PUBLIC ASIOEXT_STANDALONE)
endif ()
