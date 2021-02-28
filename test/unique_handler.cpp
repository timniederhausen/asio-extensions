#include "asioext/unique_handler.hpp"

#include "asioext/error.hpp"

#include <boost/test/unit_test.hpp>

ASIOEXT_NS_BEGIN

namespace test {

struct counters
{
  int allocations = 0;
  int deallocations = 0;
  int invocations = 0;
};

struct handler_with_allocator
{
  void operator()()
  {
    ++ctrs->invocations;
  }

  counters* ctrs;
};

template <typename T>
class counting_allocator : private std::allocator<T>
{
public:
  using value_type = T;

  counting_allocator(counters* ctrs)
    : ctrs(ctrs)
  {}

  template <typename U>
  counting_allocator(const counting_allocator<U>& other)
    : ctrs(other.ctrs)
  {}

  T* allocate(std::size_t n)
  {
    ++ctrs->allocations;
    return std::allocator<T>::allocate(n);
  }

  void deallocate(T* p, std::size_t n)
  {
    ++ctrs->deallocations;
    std::allocator<T>::deallocate(p, n);
  }

  counters* ctrs;
};

template <typename T, typename U>
constexpr bool operator==(const counting_allocator<T>& a,
                          const counting_allocator<U>& b) ASIOEXT_NOEXCEPT
{
  return a.ctrs == b.ctrs;
}

template <typename T, typename U>
constexpr bool operator!=(const counting_allocator<T>& a,
                          const counting_allocator<U>& b) ASIOEXT_NOEXCEPT
{
  return a.ctrs != b.ctrs;
}

}

ASIOEXT_NS_END

#if !defined(ASIOEXT_IS_DOCUMENTATION) && (ASIOEXT_ASIO_VERSION >= 101100)
# if defined(ASIOEXT_USE_BOOST_ASIO)
namespace boost {
# endif
namespace asio {

template <typename Allocator>
struct associated_allocator<asioext::test::handler_with_allocator, Allocator>
{
  typedef asioext::test::counting_allocator<void> type;

  static type get(const asioext::test::handler_with_allocator& h,
                  const Allocator& a = Allocator()) ASIOEXT_NOEXCEPT
  {
    return asioext::test::counting_allocator<void>(h.ctrs);
  }
};

}
# if defined(ASIOEXT_USE_BOOST_ASIO)
}
# endif
#endif

ASIOEXT_NS_BEGIN

BOOST_AUTO_TEST_SUITE(asioext_unique_handler)

BOOST_AUTO_TEST_CASE(empty)
{
  unique_handler<void ()> uh;
  BOOST_CHECK(!uh);
  uh.reset();
  BOOST_CHECK(!uh);
}

BOOST_AUTO_TEST_CASE(ctor)
{
  test::counters ctrs;
  unique_handler<void ()> uh(test::handler_with_allocator{&ctrs});
  BOOST_CHECK(uh);
  uh.complete();
  BOOST_CHECK(!uh);
  BOOST_CHECK(ctrs.allocations == 1);
  BOOST_CHECK(ctrs.deallocations == 1);
  BOOST_CHECK(ctrs.invocations == 1);
}

BOOST_AUTO_TEST_CASE(ctor_move)
{
  test::counters ctrs;
  unique_handler<void ()> uh(test::handler_with_allocator{&ctrs});
  BOOST_CHECK(uh);
  unique_handler<void ()> uh2(std::move(uh));
  BOOST_CHECK(!uh);
  BOOST_CHECK(uh2);
  uh2.complete();
  BOOST_CHECK(!uh2);
  BOOST_CHECK(ctrs.allocations == 1);
  BOOST_CHECK(ctrs.deallocations == 1);
  BOOST_CHECK(ctrs.invocations == 1);
}

BOOST_AUTO_TEST_SUITE_END()

ASIOEXT_NS_END
