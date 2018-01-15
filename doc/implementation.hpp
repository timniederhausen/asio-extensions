/// @file
/// Documents implementation detils.
///
/// @copyright Copyright (c) 2015 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/// @defgroup implementation Platform-Specific Implementation Notes
/// Notes describing platform-specific implementation details and limitations.
///
/// @section Windows
///
/// @subsection File Handles
///
/// @c CreateFileA and @c CreateFileW are used to open file handles.
///
/// @subsection win_file_io File I/O
///
/// At most one buffer may be transferred in a single operation.
///
/// @section POSIX
///
/// @subsection posix_file_handles File Handles
///
/// @c open is used to open file handles.
///
/// @subsection posix_file_io File I/O
///
/// [Asio's limitations](http://think-async.com/Asio/asio-1.11.0/doc/asio/overview/implementation.html)
/// regarding buffers apply here (most likely `min(64,IOV_MAX)` buffers per OP).
