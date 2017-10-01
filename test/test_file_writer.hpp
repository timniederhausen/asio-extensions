/// Copyright (c) 2016 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_TEST_TESTFILEWRITER_HPP
#define ASIOEXT_TEST_TESTFILEWRITER_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include <boost/filesystem.hpp>

ASIOEXT_NS_BEGIN

struct test_file_writer
{
  test_file_writer(const char* filename,
                   const void* data, std::size_t size);
  ~test_file_writer();

  boost::filesystem::path filename_;
};

ASIOEXT_NS_END

#endif
