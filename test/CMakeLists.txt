# Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

add_executable(asioext-tests)
target_sources(asioext-tests PRIVATE 
  basic_file.cpp
  chrono.cpp
  compose.cpp
  file_handle.cpp
  linear_buffer.cpp
  main.cpp
  open.cpp
  open_flags.cpp
  read_file.cpp
  test_file_rm_guard.cpp
  test_file_writer.cpp
  unique_handler.cpp
  url_parser.cpp
  write_file.cpp)
if (WIN32)
  target_sources(asioext-tests PRIVATE
    win_path.cpp
  )
endif ()

target_link_libraries(asioext-tests asioext::asioext Boost::unit_test_framework)
add_test(asioext-tests "${CMAKE_CURRENT_BUILD_DIR}/asioext-tests")
