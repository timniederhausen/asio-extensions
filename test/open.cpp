#include "test_file_rm_guard.hpp"

#if !defined(ASIOEXT_WINDOWS)
# include "scoped_umask.hpp"
#endif

#include "asioext/scoped_file_handle.hpp"
#include "asioext/open.hpp"

#if defined(ASIOEXT_USE_BOOST_ASIO)
# include <boost/asio/write.hpp>
# include <boost/asio/read.hpp>
#else
# include <asio/write.hpp>
# include <asio/read.hpp>
#endif

#include <boost/test/unit_test.hpp>
#include <boost/mpl/list.hpp>

ASIOEXT_NS_BEGIN

BOOST_AUTO_TEST_SUITE(asioext_open)

// BOOST_AUTO_TEST_SUITE() gives us a unique NS, so we don't need to
// prefix our variables.

struct char_file
{
  static const char test_filename[];

  char_file()
    : guard(test_filename)
  {
    // N.B. don't check ec here
    boost::system::error_code ec;
    boost::filesystem::remove(test_filename, ec);
  }

  test_file_rm_guard guard;
};

const char char_file::test_filename[] = "asioext_open_test";

struct wchar_file
{
  static const wchar_t test_filename[];

  wchar_file()
    : guard(test_filename)
  {
    // N.B. don't check ec here
    boost::system::error_code ec;
    boost::filesystem::remove(test_filename, ec);
  }

  test_file_rm_guard guard;
};

const wchar_t wchar_file::test_filename[] = L"asioext_open_test";

struct bfs_file
{
  static const boost::filesystem::path test_filename;

  bfs_file()
    : guard(test_filename)
  {
    // N.B. don't check ec here
    boost::system::error_code ec;
    boost::filesystem::remove(test_filename, ec);
  }

  test_file_rm_guard guard;
};

const boost::filesystem::path bfs_file::test_filename("asioext_open_test");

#if defined(ASIOEXT_WINDOWS)
typedef boost::mpl::list<char_file, wchar_file, bfs_file> file_types;
#else
typedef boost::mpl::list<char_file, bfs_file> file_types;
#endif

BOOST_AUTO_TEST_CASE_TEMPLATE(creation_dispositions, File, file_types)
{
  File f;
  asioext::error_code ec;

  auto h = asioext::open(File::test_filename,
                         open_flags::access_read |
                         open_flags::open_existing,
                         asioext::file_perms::create_default,
                         asioext::file_attrs::none, ec);

  BOOST_REQUIRE(!h.is_open());
  BOOST_REQUIRE(ec);

  h = asioext::open(File::test_filename,
                    open_flags::access_read |
                    open_flags::create_new,
                    asioext::file_perms::create_default,
                    asioext::file_attrs::none, ec);
  BOOST_CHECK(h.is_open());
  BOOST_CHECK(!ec);

  h.close();

  h = asioext::open(File::test_filename,
                    open_flags::access_read |
                    open_flags::create_new,
                    asioext::file_perms::create_default,
                    asioext::file_attrs::none, ec);
  BOOST_CHECK(!h.is_open());
  BOOST_CHECK(ec); // TODO: check for file_exists

  h = asioext::open(File::test_filename,
                    open_flags::access_read |
                    open_flags::open_always,
                    asioext::file_perms::create_default,
                    asioext::file_attrs::none, ec);
  BOOST_CHECK(h.is_open());
  BOOST_CHECK(!ec);

  h.close();

  h = asioext::open(File::test_filename,
                    open_flags::access_read |
                    open_flags::open_existing,
                    asioext::file_perms::create_default,
                    asioext::file_attrs::none, ec);
  BOOST_CHECK(h.is_open());
  BOOST_CHECK(!ec);

  h.close();

  h = asioext::open(File::test_filename,
                    open_flags::access_read |
                    open_flags::create_always,
                    asioext::file_perms::create_default,
                    asioext::file_attrs::none, ec);
  BOOST_CHECK(h.is_open());
  BOOST_CHECK(!ec);
}

BOOST_AUTO_TEST_SUITE_END()

ASIOEXT_NS_END
