/// Copyright (c) 2017 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_TEST_SCOPEDUMASK_HPP
#define ASIOEXT_TEST_SCOPEDUMASK_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include <sys/types.h>
#include <sys/stat.h>

ASIOEXT_NS_BEGIN

struct scoped_umask
{
  scoped_umask(mode_t new_mask)
    : prev_(::umask(new_mask))
  {
    // ctor
  }

  ~scoped_umask()
  {
    ::umask(prev_);
  }

  mode_t prev_;
};

ASIOEXT_NS_END

#endif
