/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_DETAIL_WINFILEOPS_HPP
#define ASIOEXT_DETAIL_WINFILEOPS_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
#pragma once
#endif

#include "asioext/detail/cstdint.hpp"

#include <asio/error_code.hpp>

#include "asioext/detail/push_options.hpp"

ASIOEXT_NS_BEGIN

namespace detail {
namespace win_file_ops {

// Pulling in windows.h/windef.h is certainly overkill.
// http://stackoverflow.com/questions/4121173/how-do-i-forward-declare-handle-win32
typedef void* handle_type;

ASIOEXT_DECL void set_error(asio::error_code& ec);

ASIOEXT_DECL handle_type open(const wchar_t* filename,
                              uint32_t flags,
                              asio::error_code& ec);

ASIOEXT_DECL void close(handle_type fd, asio::error_code& ec);

ASIOEXT_DECL handle_type duplicate(handle_type fd, asio::error_code& ec);

ASIOEXT_DECL uint64_t size(handle_type fd, asio::error_code& ec);

ASIOEXT_DECL uint64_t seek(handle_type fd, uint32_t origin, int64_t offset,
                           asio::error_code& ec);

ASIOEXT_DECL uint32_t read(handle_type fd,
                           void* buffer, uint32_t size,
                           asio::error_code& ec);

ASIOEXT_DECL uint32_t write(handle_type fd,
                            const void* buffer, uint32_t size,
                            asio::error_code& ec);

ASIOEXT_DECL uint32_t pread(handle_type fd,
                            void* buffer, uint32_t size,
                            uint64_t offset, asio::error_code& ec);

ASIOEXT_DECL uint32_t pwrite(handle_type fd,
                             const void* buffer, uint32_t size,
                             uint64_t offset, asio::error_code& ec);

}
}

ASIOEXT_NS_END

#include "asioext/detail/pop_options.hpp"

#if defined(ASIOEXT_HEADER_ONLY)
# include "asioext/detail/impl/win_file_ops.cpp"
#endif

#endif
