/// @file
/// Defines the file typedef.
///
/// @copyright Copyright (c) 2016 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_FILE_HPP
#define ASIOEXT_FILE_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include "asioext/basic_file.hpp"
#include "asioext/thread_pool_file_service.hpp"

ASIOEXT_NS_BEGIN

/// @ingroup files_handle
/// Typedef for a basic_file using the default
/// FileService (thread_pool_file_service)
typedef basic_file<thread_pool_file_service> file;

ASIOEXT_NS_END

#endif
