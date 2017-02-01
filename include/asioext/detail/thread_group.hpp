/// @copyright Copyright (c) 2017 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
//// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_DETAIL_THREADGROUP_HPP
#define ASIOEXT_DETAIL_THREADGROUP_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include "asioext/detail/thread.hpp"

ASIOEXT_NS_BEGIN

namespace detail {

class thread_group
{
public:
  thread_group()
    : last_(0)
  {
  }

  ~thread_group()
  {
    join();
  }

  template <typename Function>
  void create_thread(Function f)
  {
    last_ = new thread_state(f, last_);
  }

  template <typename Function>
  void create_threads(Function f, std::size_t num_threads)
  {
    while (num_threads-- != 0)
      last_ = new thread_state(f, last_);
  }

  void join()
  {
    while (last_) {
      thread_state* cur = last_;
      cur->t.join();
      last_ = cur->prev;
      delete cur;
    }
  }

private:
  struct thread_state
  {
    template <typename Function>
    explicit thread_state(Function f, thread_state* prev)
      : t(f)
      , prev(prev)
    {
      // ctor
    }

    thread t;
    thread_state* prev;
  };

  thread_state* last_;
};

}

ASIOEXT_NS_END

#endif
