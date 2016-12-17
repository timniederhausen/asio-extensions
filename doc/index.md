# AsioExt

AsioExt is a collection of reusable components which provide additional
high- and low-level features asio currently doesn't offer.

## List of features

* Cross-platform wrappers around native file handles with support for asio's `*Stream` concepts.
  (@ref asioext::file_handle and @ref asioext::scoped_file_handle).

## Planned features

* The `file` class which provides asynchronous file I/O support.
* Memory mapped files
* Convenience functions (read_file, write_file, etc.)
