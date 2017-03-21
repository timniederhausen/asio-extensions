/// @copyright Copyright (c) 2017 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_DETAIL_ASYNCRESULT_HPP
#define ASIOEXT_DETAIL_ASYNCRESULT_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include "asioext/detail/asio_version.hpp"

#if defined(ASIOEXT_USE_BOOST_ASIO)
# include <boost/asio/async_result.hpp>
#else
# include <asio/async_result.hpp>
#endif

#if defined(ASIOEXT_USE_BOOST_ASIO)
# define ASIOEXT_INITFN_RESULT_TYPE BOOST_ASIO_INITFN_RESULT_TYPE
#else
# define ASIOEXT_INITFN_RESULT_TYPE ASIO_INITFN_RESULT_TYPE
#endif

#include <type_traits>

ASIOEXT_NS_BEGIN

#if (ASIOEXT_ASIO_VERSION >= 101100)
using asio::async_completion;
#else
/// Backport of Asio 1.11.0's async_completion
template <typename Handler, typename Signature>
struct async_completion
{
  typedef typename asio::handler_type<
    Handler, Signature>::type handler_type;

#if defined(ASIOEXT_HAS_MOVE)
  explicit async_completion(
      Handler& orig_handler)
    : completion_handler(static_cast<typename std::conditional<
        std::is_same<Handler, handler_type>::value,
        handler_type&, Handler&&>::type>(orig_handler)),
      result(completion_handler)
  {
  }
#else
  explicit async_completion(const Handler& orig_handler)
    : completion_handler(orig_handler),
      result(completion_handler)
  {
  }
#endif

#if defined(ASIOEXT_HAS_MOVE)
  typename std::conditional<
    std::is_same<Handler, handler_type>::value,
    handler_type&, handler_type>::type completion_handler;
#else
  typename asio::handler_type<Handler, Signature>::type completion_handler;
#endif

  asio::async_result<typename asio::handler_type<Handler, Signature>
    ::type> result;
};
#endif

ASIOEXT_NS_END

#endif
