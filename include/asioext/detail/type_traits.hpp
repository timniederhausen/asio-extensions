#ifndef ASIOEXT_DETAIL_TYPETRAITS_HPP
#define ASIOEXT_DETAIL_TYPETRAITS_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
#pragma once
#endif

#if defined(ASIOEXT_HAS_STD_TYPE_TRAITS)
# include <type_traits>
#else
# include <boost/utility/enable_if.hpp>
# include <boost/type_traits/integral_constant.hpp>
#endif

ASIOEXT_NS_BEGIN

#if defined(ASIOEXT_HAS_STD_TYPE_TRAITS)
using std::enable_if;
using std::true_type;
using std::false_type;
#else
template <bool Condition, typename Type = void>
struct enable_if : boost::enable_if_c<Condition, Type> {};
using boost::true_type;
using boost::false_type;
#endif

ASIOEXT_NS_END

#endif
