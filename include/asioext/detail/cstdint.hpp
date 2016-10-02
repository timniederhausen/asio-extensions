/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_DETAIL_CSTDINT_HPP
#define ASIOEXT_DETAIL_CSTDINT_HPP

#include "asioext/detail/config.hpp"

#if defined(ASIOEXT_STANDALONE)
# include <cstdint>
#else
# include <boost/cstdint.hpp>
#endif

ASIOEXT_NS_BEGIN

#if defined(ASIOEXT_STANDALONE)
using std::int16_t;
using std::int_least16_t;
using std::int_fast16_t;
using std::uint16_t;
using std::uint_least16_t;
using std::uint_fast16_t;
using std::int32_t;
using std::int_least32_t;
using std::int_fast32_t;
using std::uint32_t;
using std::uint_least32_t;
using std::uint_fast32_t;
using std::int64_t;
using std::int_least64_t;
using std::int_fast64_t;
using std::uint64_t;
using std::uint_least64_t;
using std::uint_fast64_t;
using std::intmax_t;
using std::uintmax_t;
#else
using boost::int16_t;
using boost::int_least16_t;
using boost::int_fast16_t;
using boost::uint16_t;
using boost::uint_least16_t;
using boost::uint_fast16_t;
using boost::int32_t;
using boost::int_least32_t;
using boost::int_fast32_t;
using boost::uint32_t;
using boost::uint_least32_t;
using boost::uint_fast32_t;
# if !defined(BOOST_NO_INT64_T)
using boost::int64_t;
using boost::int_least64_t;
using boost::int_fast64_t;
using boost::uint64_t;
using boost::uint_least64_t;
using boost::uint_fast64_t;
# endif
using boost::intmax_t;
using boost::uintmax_t;
#endif

ASIOEXT_NS_END

#endif
