# Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

set(asio_source_files asio.cpp)
set(asioext_source_files asioext.cpp)

add_library(asioext.asio STATIC ${asio_source_files})
set_property(TARGET asioext.asio PROPERTY OUTPUT_NAME asio_static)
target_include_directories(asioext.asio PUBLIC ${ASIOEXT_ASIO_ROOT}/include)
target_link_libraries(asioext.asio Threads::Threads)

if ("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
	# Required for clock_gettime() (glibc < 2.17)
	target_link_libraries(asioext.asio rt)
endif ()

if (ASIOEXT_STANDALONE)
	target_compile_definitions(asioext.asio PUBLIC ASIO_STANDALONE)
	set_property(TARGET asioext.asio PROPERTY CXX_STANDARD 11)
else ()
	target_include_directories(asioext.asio PUBLIC ${Boost_INCLUDE_DIRS})
	target_compile_definitions(asioext.asio PUBLIC BOOST_ALL_NO_LIB)
	target_link_libraries(asioext.asio ${Boost_LIBRARIES})
endif ()

if (ASIOEXT_USE_BOOST_ASIO)
	target_compile_definitions(asioext.asio PUBLIC ASIOEXT_USE_BOOST_ASIO
		BOOST_ASIO_SEPARATE_COMPILATION)
else ()
	target_compile_definitions(asioext.asio PUBLIC ASIO_SEPARATE_COMPILATION)
endif ()

if (WIN32)
	target_compile_definitions(asioext.asio PUBLIC _WIN32_WINNT=0x0501)
endif ()

add_library(asioext.static STATIC ${asioext_source_files})
set_property(TARGET asioext.static PROPERTY OUTPUT_NAME asioext_static)
target_include_directories(asioext.static PUBLIC ${asioext_SOURCE_DIR}/include)
target_compile_definitions(asioext.static PUBLIC ASIOEXT_SEPARATE_COMPILATION)
target_link_libraries(asioext.static PUBLIC asioext.asio)

if (ASIOEXT_STANDALONE)
	target_compile_definitions(asioext.static PUBLIC ASIOEXT_STANDALONE)
	set_property(TARGET asioext.static PROPERTY CXX_STANDARD 11)
endif ()

if (ASIOEXT_BUILD_SHARED)
	add_library(asioext.shared SHARED ${asioext_source_files})
	set_property(TARGET asioext.shared PROPERTY OUTPUT_NAME asioext)
	target_include_directories(asioext.shared PUBLIC ${asioext_SOURCE_DIR}/include)
	target_compile_definitions(asioext.shared PUBLIC ASIOEXT_DYN_LINK PRIVATE PRIVATE ASIOEXT_SOURCE)
	target_link_libraries(asioext.shared PUBLIC asioext.asio)

	if (ASIOEXT_STANDALONE)
		target_compile_definitions(asioext.shared PUBLIC ASIOEXT_STANDALONE)
		set_property(TARGET asioext.shared PROPERTY CXX_STANDARD 11)
	endif ()
endif ()
