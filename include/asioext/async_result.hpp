/// @copyright Copyright (c) 2017 Tim Niederhausen (tim@rnc-ag.de)
/// Distributed under the Boost Software License, Version 1.0.
/// (See accompanying file LICENSE_1_0.txt or copy at
/// http://www.boost.org/LICENSE_1_0.txt)

#ifndef ASIOEXT_ASYNCRESULT_HPP
#define ASIOEXT_ASYNCRESULT_HPP

#include "asioext/detail/config.hpp"

#if ASIOEXT_HAS_PRAGMA_ONCE
# pragma once
#endif

#include "asioext/detail/asio_version.hpp"

#if defined(ASIOEXT_USE_BOOST_ASIO)
# include <boost/asio/async_result.hpp>
#else
# include <asio/async_result.hpp>
#endif

#include <type_traits>

#if defined(ASIOEXT_IS_DOCUMENTATION)
// @ingroup compat
// @brief An initiating function's return type.
//
// This is a utility macro expanding to @c async_result::return_type
// for the given completion token and signature.
//
// @note If possible @c async_result_t should be preferred.
# define ASIOEXT_INITFN_RESULT_TYPE(...) void_or_deduced
#elif defined(ASIOEXT_USE_BOOST_ASIO)
# define ASIOEXT_INITFN_RESULT_TYPE BOOST_ASIO_INITFN_RESULT_TYPE
#else
# define ASIOEXT_INITFN_RESULT_TYPE ASIO_INITFN_RESULT_TYPE
#endif

ASIOEXT_NS_BEGIN

#if (ASIOEXT_ASIO_VERSION >= 101100) && !defined(ASIOEXT_IS_DOCUMENTATION)
using asio::async_result;
using asio::async_completion;
#else
/// @ingroup compat
/// @brief An interface for customising the behaviour of an initiating function.
///
/// @note This type is the same as @c asio::async_result if Asio 1.11.0+ is
/// used.
/// For prior versions this is a compatibility type that obtains the
/// completion handler type from @c asio::handler_type and the return type from
/// @c asio::async_result.
///
/// @see asio::async_result
template <typename CompletionToken, typename Signature = void>
class async_result
{
public:
# if defined(ASIOEXT_IS_DOCUMENTATION)
  /// @brief Real completion handler type for the given @c Signature.
  typedef implementation_defined completion_handler_type;

  /// @brief Return type of the initiating function
  typedef implementation_defined return_type;
# else
  typedef typename asio::handler_type<CompletionToken, Signature>::type
    completion_handler_type;

  typedef typename asio::async_result<completion_handler_type>::type
    return_type;
# endif

  /// @brief Construct an async result from a given handler.
  explicit async_result(completion_handler_type& h)
    : result_(h)
  { }

  /// Obtain the value to be returned from the initiating function.
  return_type get()
  {
    return result_.get();
  }

  /// Initiate the asynchronous operation that will produce the result, and
  /// obtain the value to be returned from the initiating function.
  template <typename Initiation, typename RawCompletionToken, typename... Args>
  static return_type initiate(Initiation&& initiation,
                              RawCompletionToken&& token,
                              Args&&... args)
  {
    std::forward<Initiation>(initiation)(std::forward<RawCompletionToken>(token),
                                         std::forward<Args>(args)...);
  }

private:
  async_result(const async_result&) ASIOEXT_DELETED;
  async_result& operator=(const async_result&) ASIOEXT_DELETED;

  asio::async_result<completion_handler_type> result_;
};

/// @ingroup compat
/// Helper template to deduce the handler type from a CompletionToken, capture
/// a local copy of the handler, and then create an async_result for the
/// handler.
///
/// @note This type is the same as @c asio::async_completion if Asio 1.11.0+ is
/// used.
/// For prior versions this is a compatibility type that uses @c async_result
/// to obtain the needed types.
///
/// @see asio::async_completion
template <typename CompletionToken, typename Signature>
struct async_completion
{
# if defined(ASIOEXT_IS_DOCUMENTATION)
  /// @brief Type of the actual completion handler object.
  typedef implementation_defined completion_handler_type;
# else
  typedef async_result<
    typename std::decay<CompletionToken>::type,
    Signature
  > result_type;

  typedef typename result_type::completion_handler_type
    completion_handler_type;
# endif

  /// @brief Construct an @c async_completion with the given CompletionToken.
  ///
  /// This constructor initializes @c completion_handler and @c result with
  /// the given token.
  ///
  /// If @c CompletionToken and @c completion_handler_type are the same,
  /// only a reference to the token is stored, otherwise a rvalue-reference
  /// to @c token is used to construct the actual completion handler.
  explicit async_completion(CompletionToken& token)
    : completion_handler(static_cast<typename std::conditional<
        std::is_same<CompletionToken, completion_handler_type>::value,
        completion_handler_type&, CompletionToken&&>::type>(token))
    , result(completion_handler)
  {
  }

# if !defined(ASIOEXT_IS_DOCUMENTATION)
  typename std::conditional<
    std::is_same<CompletionToken, completion_handler_type>::value,
    completion_handler_type&, completion_handler_type
  >::type completion_handler;
# else
  /// @brief The actual completion handler for the asynchronous operation.
  completion_handler_type completion_handler;
# endif

  /// @brief Result object for the initiating function's caller.
  result_type result;
};
#endif

#if defined(ASIOEXT_IS_DOCUMENTATION)
/// @brief Initiate an asynchronous operation.
///
/// This helper function takes care of the necessary boilerplate
/// needed to initiate an asynchronous operation.
///
/// First, it creates a completion handler based on the given completion token.
/// Afterwards it invokes the provided @c Initiation callable with an rvalue
/// reference to the completion handler, as well as the
/// additional arguments passed to this function.
///
/// The behaviour of this function can be customized by providing
/// the @ref async_result::initiate member function in your specialization.
///
/// @param initiation Initiation function which receives an rvalue reference
/// to the instantiated completion handler, as well as @c args...
/// @param token The opaque completion token.
/// @param args Additional arguments for @c initiation
/// @returns result of the started asynchronous operation.
/// This is completion token-dependent
/// (e.g. @c std::future for @c asio::use_future)
template <typename CompletionToken, typename Signature,
          typename Initiation, typename... Args>
ASIOEXT_INITFN_RESULT_TYPE(CompletionToken, Signature)
async_initiate(Initiation&& initiation,
               CompletionToken& token,
               Args&&... args);
#elif (ASIOEXT_ASIO_VERSION >= 101300)
using asio::async_initiate;
#else
template <typename CompletionToken, typename Signature>
struct has_initiation_function
{
  typedef char (& yes)[1];
  typedef char (& no)[2];

  template <typename U> static yes check(decltype(&U::initiate));
  template <typename> static no check(...);

  static bool const value = sizeof(check<
    async_result<typename std::decay<CompletionToken>::type, Signature>
  >(0)) == sizeof(yes);
};

template <typename CompletionToken, typename Signature,
          typename Initiation, typename... Args>
inline typename std::enable_if<
    has_initiation_function<CompletionToken, Signature>::value,
    ASIOEXT_INITFN_RESULT_TYPE(CompletionToken, Signature)
>::type async_initiate(Initiation&& initiation,
                       CompletionToken& token,
                       Args&&... args)
{
  return async_result<typename decay<CompletionToken>::type,
    Signature>::initiate(std::forward<Initiation>(initiation),
      token, std::forward<Args>(args)...);
}

template <typename CompletionToken, typename Signature,
          typename Initiation, typename... Args>
inline typename std::enable_if<
    !has_initiation_function<CompletionToken, Signature>::value,
    ASIOEXT_INITFN_RESULT_TYPE(CompletionToken, Signature)
>::type async_initiate(Initiation&& initiation,
                       CompletionToken& token,
                       Args&&... args)
{
  typedef async_completion<CompletionToken, Signature> completion_type;
  typedef typename completion_type::completion_handler_type handler_type;

  completion_type init(token);
  std::forward<Initiation>(initiation)(
      std::move(init.completion_handler),
      std::forward<Args>(args)...);
  return init.result.get();
}
#endif

#if defined(ASIOEXT_HAS_ALIAS_TEMPLATES)
/// @ingroup compat
/// @brief An initiating function's return type.
///
/// This alias template refers to the @c async_result::return_type
/// type for the specified CompletionToken and Signature.
template <typename CompletionToken, typename Signature>
using async_result_t = typename async_result<
  typename std::decay<CompletionToken>::type, Signature
>::return_type;

/// @ingroup compat
/// @brief The real handler type to be used for the asynchronous operation.
///
/// This alias template refers to the @c async_result::completion_handler_type
/// type for the specified CompletionToken and Signature.
template <typename CompletionToken, typename Signature>
using completion_handler_t = typename async_result<
  typename std::decay<CompletionToken>::type, Signature
>::completion_handler_type;
#endif

ASIOEXT_NS_END

#endif
