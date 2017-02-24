# Introduction

AsioExt is a collection of components which provide additional
high- and low-level features asio currently doesn't offer.

## List of features

* Wrappers around native file handles with support for asio's `*Stream` concepts.
  (@ref asioext::file_handle and @ref asioext::scoped_file_handle).
* Asynchronous file I/O (@ref asioext::basic_file).
  Currently only a thread(-pool) service is implemented
  (@ref asioext::thread_pool_file_service).
* Utilities for reading/writing files:
  * @ref read_file
  * @ref write_file
* Utilities for service/custom I/O object writers (@ref service).
