# Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

# Header-only Asio target
add_library(asioext.asio-hdr INTERFACE)
target_compile_features(asioext.asio-hdr INTERFACE cxx_std_17)
target_link_libraries(asioext.asio-hdr INTERFACE Threads::Threads)

if ("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
	# Required for clock_gettime() (glibc < 2.17)
	target_link_libraries(asioext.asio-hdr INTERFACE rt)
endif ()

if (ASIOEXT_USE_BOOST_ASIO)
	target_compile_definitions(asioext.asio-hdr INTERFACE ASIOEXT_USE_BOOST_ASIO)
else ()
	target_include_directories(asioext.asio-hdr INTERFACE ${ASIOEXT_ASIO_ROOT}/include)
endif ()

if (ASIOEXT_STANDALONE)
	target_compile_definitions(asioext.asio-hdr INTERFACE ASIO_STANDALONE)
else ()
	target_include_directories(asioext.asio-hdr INTERFACE ${Boost_INCLUDE_DIRS})
	target_compile_definitions(asioext.asio-hdr INTERFACE BOOST_ALL_NO_LIB)
	target_link_libraries(asioext.asio-hdr INTERFACE ${Boost_LIBRARIES})
endif ()

if (WIN32)
	if (WINDOWS_STORE OR NOT ASIOEXT_WINDOWS_XP)
		target_compile_definitions(asioext.asio-hdr INTERFACE _WIN32_WINNT=0x0A00)
	else ()
		target_compile_definitions(asioext.asio-hdr INTERFACE _WIN32_WINNT=0x0501)
	endif ()
endif ()

# Static/shared library target for Asio
# TODO: This should probably be imported from somewhere(?)
add_library(asioext.asio-lib STATIC asio.cpp)
set_target_properties(asioext.asio-lib PROPERTIES OUTPUT_NAME asio)
target_link_libraries(asioext.asio-lib PUBLIC asioext.asio-hdr)

if (BUILD_SHARED_LIBS)
	target_compile_definitions(asioext.asio-lib PUBLIC ASIO_DYN_LINK)
endif ()

if (ASIOEXT_USE_BOOST_ASIO)
	target_compile_definitions(asioext.asio-lib PUBLIC BOOST_ASIO_SEPARATE_COMPILATION)
else ()
	target_compile_definitions(asioext.asio-lib PUBLIC ASIO_SEPARATE_COMPILATION)
endif ()

# Header-only target for asioext
add_library(asioext-hdr INTERFACE)
target_compile_features(asioext-hdr INTERFACE cxx_std_17)
target_include_directories(asioext-hdr INTERFACE ${asioext_SOURCE_DIR}/include)
target_link_libraries(asioext-hdr INTERFACE asioext.asio-hdr)

if (ASIOEXT_STANDALONE)
	target_compile_definitions(asioext-hdr INTERFACE ASIOEXT_STANDALONE)
endif ()

if (ASIOEXT_WINDOWS_USE_UTF8_FILENAMES)
	# TODO: not strictly necessary, this could be impl. only!
	target_compile_definitions(asioext-hdr INTERFACE ASIOEXT_WINDOWS_USE_UTF8_FILENAMES)
endif ()

# Static/shared library target for asioext
add_library(asioext-lib asioext.cpp)
target_compile_definitions(asioext-lib PUBLIC ASIOEXT_SEPARATE_COMPILATION)
target_link_libraries(asioext-lib PUBLIC asioext-hdr asioext.asio-lib)

if (BUILD_SHARED_LIBS)
	target_compile_definitions(asioext PUBLIC ASIOEXT_DYN_LINK)
endif ()

# Add aliases for the old target names, now referring to either
# the header-only or the compiled version.
if (ASIOEXT_HEADER_ONLY)
	add_library(asioext.asio ALIAS asioext.asio-hdr)
	add_library(asioext ALIAS asioext-hdr)
	add_library(AsioExt::asioext ALIAS asioext-hdr)
else ()
	add_library(asioext.asio ALIAS asioext.asio-lib)
	add_library(asioext ALIAS asioext-lib)
	add_library(AsioExt::asioext ALIAS asioext-lib)
endif ()
