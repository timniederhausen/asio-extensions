# Asio Extensions

_Asio Extensions_ is a collection of reusable components which provide additional
high- and low-level features asio currently doesn't offer.

For now it only works with the standalone asio version, although support for
_Boost.Asio_ is planned.

## List of features

* The @ref asioext::file_handle class which simply wraps the current platform's native file handle
   and provides support for Asio's `*Stream` concepts on top of it.
