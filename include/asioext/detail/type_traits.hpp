#ifndef ASIOEXT_DETAIL_TYPETRAITS_HPP
#define ASIOEXT_DETAIL_TYPETRAITS_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
#pragma once
#endif

# include <type_traits>

ASIOEXT_NS_BEGIN

using std::enable_if;
using std::conditional;
using std::is_same;
using std::true_type;
using std::false_type;
using std::underlying_type;

ASIOEXT_NS_END

#endif
