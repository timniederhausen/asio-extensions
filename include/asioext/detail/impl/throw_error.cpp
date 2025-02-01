/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "asioext/detail/throw_error.hpp"
#include "asioext/detail/throw_exception.hpp"

#if defined(ASIOEXT_USE_BOOST_ASIO)
# include <boost/system/system_error.hpp>
#else
# if !defined(ASIO_HAS_STD_SYSTEM_ERROR)
#  error Missing standard library error codes (<system_error>)
# endif
# include <system_error>
#endif

ASIOEXT_NS_BEGIN

namespace detail {

ASIOEXT_DECL void do_throw_error(const error_code& ec)
{
#if defined(ASIOEXT_USE_BOOST_ASIO)
  using boost::system::system_error;
#else
  using std::system_error;
#endif

  system_error err(ec);
  asioext::detail::throw_exception(err);
}

ASIOEXT_DECL void do_throw_error(const error_code& ec, const char* location)
{
#if defined(ASIOEXT_USE_BOOST_ASIO)
  using boost::system::system_error;
#else
  using std::system_error;
#endif

  system_error err(ec, location);
  asioext::detail::throw_exception(err);
}

}

ASIOEXT_NS_END
