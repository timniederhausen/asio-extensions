# Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

set(asio_source_files asio.cpp)
set(asioext_source_files asioext.cpp)

add_library(asioext.asio ${asio_source_files})
set_target_properties(asioext.asio PROPERTIES OUTPUT_NAME asio CXX_STANDARD 11)
target_include_directories(asioext.asio PUBLIC ${ASIOEXT_ASIO_ROOT}/include)
target_link_libraries(asioext.asio Threads::Threads)

if (BUILD_SHARED_LIBS)
	target_compile_definitions(asioext.asio PUBLIC ASIO_DYN_LINK)
endif ()

if ("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
	# Required for clock_gettime() (glibc < 2.17)
	target_link_libraries(asioext.asio rt)
endif ()

if (ASIOEXT_STANDALONE)
	target_compile_definitions(asioext.asio PUBLIC ASIO_STANDALONE)
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
	if (WINDOWS_STORE OR NOT ASIOEXT_WINDOWS_XP)
		target_compile_definitions(asioext.asio PUBLIC _WIN32_WINNT=0x0A00)
	else ()
		target_compile_definitions(asioext.asio PUBLIC _WIN32_WINNT=0x0501)
	endif ()
endif ()

add_library(asioext ${asioext_source_files})
target_compile_features(asioext PUBLIC cxx_std_17)
target_include_directories(asioext PUBLIC ${asioext_SOURCE_DIR}/include)
target_compile_definitions(asioext PUBLIC ASIOEXT_SEPARATE_COMPILATION)
target_link_libraries(asioext PUBLIC asioext.asio)

if (BUILD_SHARED_LIBS)
	target_compile_definitions(asioext PUBLIC ASIOEXT_DYN_LINK)
endif ()

if (ASIOEXT_STANDALONE)
	target_compile_definitions(asioext PUBLIC ASIOEXT_STANDALONE)
endif ()

if (ASIOEXT_WINDOWS_USE_UTF8_FILENAMES)
	target_compile_definitions(asioext PRIVATE ASIOEXT_WINDOWS_USE_UTF8_FILENAMES)
endif ()
