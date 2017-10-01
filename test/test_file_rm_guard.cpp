/// Copyright (c) 2017 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#include "test_file_rm_guard.hpp"

#include <iostream>

ASIOEXT_NS_BEGIN

test_file_rm_guard::~test_file_rm_guard()
{
  if (filename_.empty())
    return;

  boost::system::error_code ec;
  boost::filesystem::remove(filename_, ec);
  if (ec) {
    std::cerr << "Failed to delete " << filename_ << " with " << ec;
    std::cerr << std::endl;
    std::abort();
  }
}

ASIOEXT_NS_END
