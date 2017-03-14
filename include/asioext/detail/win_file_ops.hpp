/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_DETAIL_WINFILEOPS_HPP
#define ASIOEXT_DETAIL_WINFILEOPS_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include "asioext/open_flags.hpp"
#include "asioext/seek_origin.hpp"
#include "asioext/file_perms.hpp"
#include "asioext/file_attrs.hpp"

#include "asioext/detail/cstdint.hpp"
#include "asioext/detail/error_code.hpp"

ASIOEXT_NS_BEGIN

namespace detail {
namespace win_file_ops {

// Pulling in windows.h/windef.h is certainly overkill.
// http://stackoverflow.com/questions/4121173/how-do-i-forward-declare-handle-win32
typedef void* handle_type;

struct create_file_args;

ASIOEXT_DECL void set_error(error_code& ec);

ASIOEXT_DECL bool parse_open_flags(create_file_args& args,
                                   open_flags flags,
                                   file_perms perms, file_attrs attrs);

ASIOEXT_DECL handle_type open(const char* filename,
                              open_flags flags,
                              file_perms perms, file_attrs attrs,
                              error_code& ec);

ASIOEXT_DECL handle_type open(const wchar_t* filename,
                              open_flags flags,
                              file_perms perms, file_attrs attrs,
                              error_code& ec);

ASIOEXT_DECL void close(handle_type fd, error_code& ec);

ASIOEXT_DECL handle_type duplicate(handle_type fd, error_code& ec);

ASIOEXT_DECL handle_type get_stdin(error_code& ec);
ASIOEXT_DECL handle_type get_stdout(error_code& ec);
ASIOEXT_DECL handle_type get_stderr(error_code& ec);

ASIOEXT_DECL uint64_t size(handle_type fd, error_code& ec);

ASIOEXT_DECL uint64_t seek(handle_type fd,
                           seek_origin origin,
                           int64_t offset,
                           error_code& ec);

ASIOEXT_DECL uint32_t read(handle_type fd,
                           void* buffer,
                           uint32_t size,
                           error_code& ec);

ASIOEXT_DECL uint32_t write(handle_type fd,
                            const void* buffer,
                            uint32_t size,
                            error_code& ec);

ASIOEXT_DECL uint32_t pread(handle_type fd,
                            void* buffer,
                            uint32_t size,
                            uint64_t offset,
                            error_code& ec);

ASIOEXT_DECL uint32_t pwrite(handle_type fd,
                             const void* buffer,
                             uint32_t size,
                             uint64_t offset,
                             error_code& ec);

}
}

ASIOEXT_NS_END

#if defined(ASIOEXT_HEADER_ONLY)
# include "asioext/detail/impl/win_file_ops.cpp"
#endif

#endif
