/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "asioext/detail/config.hpp"

#include "asioext/impl/file_handle.cpp"
#include "asioext/impl/scoped_file_handle.cpp"
#include "asioext/impl/open.cpp"
#include "asioext/impl/open_flags.cpp"

#if defined(ASIOEXT_WINDOWS)
# include "asioext/impl/file_handle_win.cpp"
# include "asioext/impl/scoped_file_handle_win.cpp"
# include "asioext/detail/impl/win_path.cpp"
# include "asioext/detail/impl/win_file_ops.cpp"
#else
# include "asioext/impl/file_handle_posix.cpp"
# include "asioext/impl/scoped_file_handle_posix.cpp"
# include "asioext/detail/impl/posix_file_ops.cpp"
#endif
