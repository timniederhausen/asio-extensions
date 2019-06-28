#include "asioext/compose.hpp"

#if (ASIOEXT_ASIO_VERSION >= 101100)
# if defined(ASIOEXT_USE_BOOST_ASIO)
#  include <boost/asio/io_context.hpp>
# else
#  include <asio/io_context.hpp>
# endif
#endif

#include <boost/test/unit_test.hpp>
#include <boost/mpl/list.hpp>

#include <cstdlib>

ASIOEXT_NS_BEGIN

BOOST_AUTO_TEST_SUITE(asioext_composed_operation)

struct simple_handler
{
  void operator()() {}

#if (ASIOEXT_ASIO_VERSION >= 101100)
  typedef std::allocator<void> allocator_type;
  typedef asio::io_context::executor_type executor_type;

  executor_type get_executor() const ASIOEXT_NOEXCEPT
  {
    ++get_executor_count;
    static asio::io_context ctx;
    return ctx.get_executor();
  }

  allocator_type get_allocator() const ASIOEXT_NOEXCEPT
  {
    ++get_allocator_count;
    return allocator_type();
  }

  mutable int get_executor_count = 0;
  mutable int get_allocator_count = 0;
#endif
};

#if (ASIOEXT_ASIO_VERSION >= 101100)
struct test_make_composed_op_operation
{
  template <typename Self>
  void operator()(Self&& self)
  {
    BOOST_CHECK_EQUAL(2, self.get_handler().get_executor_count);
    BOOST_CHECK_EQUAL(1, self.get_handler().get_allocator_count);
  }
};

template <typename T>
static void test_make_composed_op_aux(T h)
{
  static_assert(std::is_same<std::allocator<void>,
                typename asio::associated_allocator<T>::type>::value,
                "same allocator");
  static_assert(std::is_same<asio::io_context::executor_type,
                typename asio::associated_executor<T>::type>::value,
                "same executor");

  asio::associated_executor<T>::get(h);
  asio::associated_executor<T>::get(h);
  asio::associated_allocator<T>::get(h);

  h();
}

BOOST_AUTO_TEST_CASE(make_composed_op)
{
  test_make_composed_op_aux(make_composed_operation(
    test_make_composed_op_operation(), simple_handler()));
}
#endif

BOOST_AUTO_TEST_SUITE_END()

ASIOEXT_NS_END
