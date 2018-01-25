# Introduction

AsioExt is a collection of components that build on Asio.
It is compatible with standalone Asio, as well as the Boost version.

## Feature overview

* The @ref files component provides platform-independent functionality
  that is not provided by Boost.Filesystem or standard iostreams.
* The @ref net component contains utilities that make using Asio objects
  more convenient.
* The @ref core component provides low-level utilities that are used in other
  components (e.g. cancellation tokens)

## Building

AsioExt requires a compiler that supports at least some C++11 features:

* `constexpr` functions
* Scoped enumerations (`enum class`)
* C++11 `<type_traits>`
* rvalue references (optional, if unavailable, objects will not be movable).

AsioExt's behavior can be customized by defining certain macros.
See @ref buildopts for a full list of possible build options.

Like Asio, AsioExt is header-only by default.
However, it is also possible to build AsioExt as a separate compilation unit.

Currently, AsioExt has support for CMake as well as GN.

## Supported platforms

Currently, AsioExt is being tested on the following platforms:

* Linux
* mac OS
* Windows + Universal Windows

## Dependencies

* Asio 1.10.0+ (Only if not using Boost.Asio)
* [optional] Boost 1.46.0+ (If _Boost.Filesystem_ or _Boost.Asio_ is used)
