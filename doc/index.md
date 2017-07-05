# Introduction

AsioExt is a collection of components that build on Asio.
It is compatible with standalone Asio, as well as the Boost version.

## Feature list

* File handle wrappers (@ref asioext::file_handle, @ref asioext::scoped_file_handle) with support for:
  * Creating and opening files
  * Asio's `*Stream` concepts (*SyncReadStream*, *SyncRandomAccessReadDevice*, ...)
  * File permissions
  * File attributes
* Asynchronous file I/O interface (@ref asioext::basic_file) with the following implementations:
  * @ref asioext::thread_pool_file_service : Blocking I/O operations are performed on the
     thread-pool.
* Utilities for reading/writing files:
  * @ref asioext::read_file
  * @ref asioext::write_file
* Utilities for service writers (@ref service):
  * Cancellation tokens (@ref asioext::cancellation_token)
  * `Handler` wrappers (@ref asioext::composed_operation)

## Building

AsioExt requires a compiler that supports at least some C++11 features:

* `constexpr` functions
* Scoped enumerations (`enum class`)
* C++11 `<type_traits>`
* R-value references (optional, if unavailable, objects will not be movable).

Like Asio, AsioExt is header-only by default.
However, it is also possible to build AsioExt as a separate compilation unit.

See @ref buildopts for a full list of possible build options.

## Supported platforms

Currently, AsioExt is being tested on the following platforms:

* Linux (GCC 4.8, Clang 3.6)
* mac OS (Xcode 7.3.1 - Apple Clang 7.3.0)
* Windows (MSVC 2015)

## Dependencies

* Asio 1.10.0+ (Only if not using Boost.Asio)
* [optional] Boost 1.46.0+ (If _Boost.Filesystem_ or _Boost.Asio_ is used)
