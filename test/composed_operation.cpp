#include "asioext/composed_operation.hpp"

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

  int allocate_count = 0;
  int deallocate_count = 0;
  int is_continuation_count = 0;
  int invoke_count = 0;

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

void* asio_handler_allocate(std::size_t size,
    simple_handler* this_handler)
{
  ++this_handler->allocate_count;
  return std::malloc(size);
}

void asio_handler_deallocate(void* pointer, std::size_t size,
    simple_handler* this_handler)
{
  ++this_handler->deallocate_count;
  std::free(pointer);
}

bool asio_handler_is_continuation(
    simple_handler* this_handler)
{
  ++this_handler->is_continuation_count;
  return false;
}

template <typename Function>
void asio_handler_invoke(Function& function,
                         simple_handler* this_handler)
{
  ++this_handler->invoke_count;
  function();
}

template <typename Function>
void asio_handler_invoke(const Function& function,
                         simple_handler* this_handler)
{
  ++this_handler->invoke_count;
  function();
}

static void test_function()
{
  // no op
}

template <class Handler>
struct simple_composed_operation : asioext::composed_operation<Handler>
{
  simple_composed_operation(Handler&& h)
    : asioext::composed_operation<Handler>(std::move(h))
  {}
};

typedef boost::mpl::list<
  //simple_handler,
  asioext::composed_operation<simple_handler>,
  simple_composed_operation<simple_handler>
> handler_types;

BOOST_AUTO_TEST_CASE_TEMPLATE(op_base_class_hooks, test_type, handler_types)
{
  test_type h = test_type(simple_handler());

  // test allocation hooks
  ASIOEXT_HANDLER_ALLOC_HELPERS_NS::deallocate(
      ASIOEXT_HANDLER_ALLOC_HELPERS_NS::allocate(1024, h), 1024, h);
  BOOST_CHECK_EQUAL(1, h.handler_.allocate_count);
  BOOST_CHECK_EQUAL(1, h.handler_.deallocate_count);

  BOOST_CHECK(!ASIOEXT_HANDLER_CONT_HELPERS_NS::is_continuation(h));
  BOOST_CHECK(!ASIOEXT_HANDLER_CONT_HELPERS_NS::is_continuation(h));
  BOOST_CHECK_EQUAL(2, h.handler_.is_continuation_count);

  ASIOEXT_HANDLER_INVOKE_HELPERS_NS::invoke(&test_function, h);
  BOOST_CHECK_EQUAL(1, h.handler_.invoke_count);

  test_type h2 = h;
  ASIOEXT_HANDLER_INVOKE_HELPERS_NS::invoke(&test_function, h2);
  BOOST_CHECK_EQUAL(1, h.handler_.invoke_count);
  BOOST_CHECK_EQUAL(2, h2.handler_.invoke_count);
}

#if (ASIOEXT_ASIO_VERSION >= 101100)
struct test_make_composed_op_operation
{
  template <typename Handler>
  void operator()(Handler h)
  {
    BOOST_CHECK_EQUAL(2, h.get_executor_count);
    BOOST_CHECK_EQUAL(1, h.get_allocator_count);
    BOOST_CHECK_EQUAL(1, h.allocate_count);
    BOOST_CHECK_EQUAL(1, h.deallocate_count);
    BOOST_CHECK_EQUAL(2, h.is_continuation_count);
    BOOST_CHECK_EQUAL(1, h.invoke_count);
  }
};

template <typename T>
static void test_make_composed_op_aux(T h)
{
  ASIOEXT_HANDLER_ALLOC_HELPERS_NS::deallocate(
      ASIOEXT_HANDLER_ALLOC_HELPERS_NS::allocate(1024, h), 1024, h);
  BOOST_CHECK(!ASIOEXT_HANDLER_CONT_HELPERS_NS::is_continuation(h));
  BOOST_CHECK(!ASIOEXT_HANDLER_CONT_HELPERS_NS::is_continuation(h));
  ASIOEXT_HANDLER_INVOKE_HELPERS_NS::invoke(&test_function, h);

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
    simple_handler(), test_make_composed_op_operation()));
}
#endif

BOOST_AUTO_TEST_SUITE_END()

ASIOEXT_NS_END
