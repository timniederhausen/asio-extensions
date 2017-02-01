/// @copyright Copyright (c) 2017 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_DETAIL_WORK_HPP
#define ASIOEXT_DETAIL_WORK_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include "asioext/detail/asio_version.hpp"

#if defined(ASIOEXT_USE_BOOST_ASIO)
# include <boost/asio/io_service.hpp>
#else
# include <asio/io_service.hpp>
#endif

ASIOEXT_NS_BEGIN

namespace detail {

// Helper class for fine-grained work creation.
class work
{
public:
  work(asio::io_service& io_service)
    : io_service_(io_service)
#if ASIOEXT_ASIO_VERSION < 101100
    , impl_(asio::use_service<asio::detail::io_service_impl>(io_service))
#endif
  {
    // ctor
  }

  void on_work_started()
  {
#if ASIOEXT_ASIO_VERSION >= 101100
    io_service_.get_executor().on_work_started();
#else
    impl_.work_started();
#endif
  }

  void on_work_finished()
  {
#if ASIOEXT_ASIO_VERSION >= 101100
    io_service_.get_executor().on_work_finished();
#else
    impl_.work_finished();
#endif
  }

private:
  work(const work&) ASIOEXT_DELETED;
  work& operator=(const work&) ASIOEXT_DELETED;

  asio::io_service& io_service_;
#if ASIOEXT_ASIO_VERSION < 101100
  asio::detail::io_service_impl& impl_;
#endif
};

}

ASIOEXT_NS_END

#endif
