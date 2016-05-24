# Asio Extensions

_Asio Extensions_ is a collection of reusable components which provide additional
high- and low-level features asio currently doesn't offer.

For now it only works with the standalone asio version, although support for
_Boost.Asio_ is planned.

## List of features

* @ref asioext::file_handle A simple wrapper for native file handles
   that supports Asio's `*Stream` concepts.
