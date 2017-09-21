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

std::ostream& operator<<(std::ostream& os, file_attrs attrs)
{
   os << static_cast<std::underlying_type<file_perms>::type>(attrs);
   return os;
}

std::ostream& operator<<(std::ostream& os, file_perms perms)
{
   os << static_cast<std::underlying_type<file_perms>::type>(perms);
   return os;
}

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
  File delete_guard;
  asioext::error_code ec;

  auto h = asioext::open(File::test_filename,
                         open_flags::access_read |
                         open_flags::open_existing,
                         asioext::file_perms::create_default,
                         asioext::file_attrs::none, ec);
  BOOST_REQUIRE(ec);
  BOOST_CHECK(!h.is_open());

  h = asioext::open(File::test_filename,
                    open_flags::access_read |
                    open_flags::create_new,
                    asioext::file_perms::create_default,
                    asioext::file_attrs::none, ec);
  BOOST_REQUIRE_MESSAGE(!ec, "ec: " << ec);
  BOOST_CHECK(h.is_open());

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
  BOOST_REQUIRE_MESSAGE(!ec, "ec: " << ec);
  BOOST_CHECK(h.is_open());

  h.close();

  h = asioext::open(File::test_filename,
                    open_flags::access_read |
                    open_flags::open_existing,
                    asioext::file_perms::create_default,
                    asioext::file_attrs::none, ec);
  BOOST_REQUIRE_MESSAGE(!ec, "ec: " << ec);
  BOOST_CHECK(h.is_open());

  h.close();

  h = asioext::open(File::test_filename,
                    open_flags::access_read |
                    open_flags::create_always,
                    asioext::file_perms::create_default,
                    asioext::file_attrs::none, ec);
  BOOST_REQUIRE_MESSAGE(!ec, "ec: " << ec);
  BOOST_CHECK(h.is_open());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(permissions, File, file_types)
{
  asioext::error_code ec;

#if !defined(ASIOEXT_WINDOWS)
  // Ensure the process' umask doesn't interfere with our permission tests.
  scoped_umask umsk(0);

  static constexpr file_perms permissions_to_test[] = {
    asioext::file_perms::owner_read,
    asioext::file_perms::owner_write,
    asioext::file_perms::owner_read | asioext::file_perms::owner_write,
    asioext::file_perms::owner_all,

    asioext::file_perms::group_read,
    asioext::file_perms::group_write,
    asioext::file_perms::group_read | asioext::file_perms::owner_write,
    asioext::file_perms::group_all,

    asioext::file_perms::others_read,
    asioext::file_perms::others_write,
    asioext::file_perms::others_read | asioext::file_perms::owner_write,
    asioext::file_perms::others_all,

    asioext::file_perms::all,
  };

  {
    File delete_guard;
    auto h = asioext::open(File::test_filename,
                           open_flags::access_write |
                           open_flags::create_new,
                           asioext::file_perms::create_default,
                           asioext::file_attrs::none, ec);
    BOOST_REQUIRE(h.is_open());
    BOOST_REQUIRE_MESSAGE(!ec, "ec: " << ec);
    for (file_perms perms : permissions_to_test) {
      BOOST_REQUIRE_NO_THROW(h.permissions(perms));
      BOOST_REQUIRE_EQUAL(h.permissions(), perms);
    }
  }

  for (file_perms perms : permissions_to_test) {
    test_file_rm_guard delete_guard(File::test_filename);
    auto h = asioext::open(File::test_filename,
                           open_flags::access_read |
                           open_flags::create_new,
                           perms,
                           asioext::file_attrs::none, ec);

    BOOST_REQUIRE(h.is_open());
    BOOST_REQUIRE_MESSAGE(!ec, "ec: " << ec);
    BOOST_REQUIRE_EQUAL(h.permissions(), perms);
  }

  test_file_rm_guard delete_guard(File::test_filename);

#else
  File delete_guard;

  constexpr asioext::file_perms write_perms =
      asioext::file_perms::owner_write |
      asioext::file_perms::group_write |
      asioext::file_perms::others_write;

  auto h = asioext::open(File::test_filename,
                         open_flags::access_write |
                         open_flags::create_new,
                         asioext::file_perms::create_default & ~write_perms,
                         asioext::file_attrs::none, ec);

  BOOST_REQUIRE(h.is_open());
  BOOST_REQUIRE_MESSAGE(!ec, "ec: " << ec);
  BOOST_REQUIRE_EQUAL(h.permissions(),
                      (asioext::file_perms::all & ~write_perms));

  BOOST_REQUIRE_NO_THROW(h.permissions(asioext::file_perms::all));
  BOOST_REQUIRE_EQUAL(h.permissions(),
                      asioext::file_perms::all);
#endif
}

BOOST_AUTO_TEST_CASE_TEMPLATE(attributes, File, file_types)
{
  asioext::error_code ec;

#if !defined(ASIOEXT_WINDOWS)
  // Ensure the process' umask doesn't interfere with our permission tests.
  scoped_umask umsk(0);

  static constexpr file_attrs attributes_to_test[] = {
#ifdef UF_HIDDEN
    file_attrs::hidden,
#endif
#ifdef UF_SYSTEM
    file_attrs::system,
#endif
#ifdef UF_ARCHIVE
    file_attrs::archive,
#endif
#ifdef UF_NODUMP
    file_attrs::no_dump,
#endif
#ifdef UF_IMMUTABLE
    file_attrs::user_immutable,
#endif
#ifdef UF_NOUNLINK
    file_attrs::user_no_unlink,
#endif
  };
#else
  static constexpr asioext::file_attrs attributes_to_test[] = {
    file_attrs::hidden,
    file_attrs::system,
    // Might already be set by default (if inherited)
    //file_attrs::not_indexed,
  };
#endif

  {
    File delete_guard;
    auto h = asioext::open(File::test_filename,
                           open_flags::access_write |
                           open_flags::create_new,
                           asioext::file_perms::create_default,
                           asioext::file_attrs::none, ec);
    BOOST_REQUIRE(h.is_open());
    BOOST_REQUIRE_MESSAGE(!ec, "ec: " << ec);
    for (file_attrs attrs : attributes_to_test) {
      BOOST_REQUIRE_NO_THROW(h.attributes(attrs));
      BOOST_REQUIRE_EQUAL(h.attributes() & ~file_attrs::not_indexed, attrs);
    }
    // Remove all attributes again. Some (immutable, ...) might otherwise
    // prevent the file's deletion.
    h.attributes(file_attrs::none, ec);
    BOOST_REQUIRE_MESSAGE(!ec, "ec: " << ec);
  }

  for (file_attrs attrs : attributes_to_test) {
    test_file_rm_guard delete_guard(File::test_filename);
    auto h = asioext::open(File::test_filename,
                           open_flags::access_write |
                           open_flags::create_new,
                           asioext::file_perms::create_default,
                           attrs, ec);

    BOOST_REQUIRE(h.is_open());
    BOOST_REQUIRE_MESSAGE(!ec, "ec: " << ec);
    BOOST_REQUIRE_EQUAL(h.attributes() & ~file_attrs::not_indexed, attrs);
    // Remove all attributes again. Some (immutable, ...) might otherwise
    // prevent the file's deletion.
    h.attributes(file_attrs::none, ec);
    BOOST_CHECK_MESSAGE(!ec, "ec: " << ec);
  }
}

BOOST_AUTO_TEST_SUITE_END()

ASIOEXT_NS_END
