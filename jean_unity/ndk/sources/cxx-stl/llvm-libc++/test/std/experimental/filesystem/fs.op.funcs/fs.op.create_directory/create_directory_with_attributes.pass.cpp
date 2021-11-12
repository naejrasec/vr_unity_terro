//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++98, c++03

// <experimental/filesystem>

// bool create_directory(const path& p, const path& attr);
// bool create_directory(const path& p, const path& attr, error_code& ec) noexcept;

#include <experimental/filesystem>
#include <type_traits>
#include <cassert>

#include "test_macros.h"
#include "rapid-cxx-test.hpp"
#include "filesystem_test_helper.hpp"

using namespace std::experimental::filesystem;
namespace fs = std::experimental::filesystem;

TEST_SUITE(filesystem_create_directory_test_suite)

TEST_CASE(test_signatures)
{
    const path p; ((void)p);
    std::error_code ec; ((void)ec);
    ASSERT_SAME_TYPE(decltype(fs::create_directory(p, p)), bool);
    ASSERT_SAME_TYPE(decltype(fs::create_directory(p, p, ec)), bool);
    ASSERT_NOT_NOEXCEPT(fs::create_directory(p, p));
    ASSERT_NOEXCEPT(fs::create_directory(p, p, ec));
}

TEST_CASE(create_existing_directory)
{
    scoped_test_env env;
    const path dir = env.create_dir("dir1");
    const path dir2 = env.create_dir("dir2");

    const perms orig_p = status(dir).permissions();
    permissions(dir2, perms::none);

    std::error_code ec;
    TEST_CHECK(fs::create_directory(dir, dir2, ec) == false);
    TEST_CHECK(!ec);

    // Check that the permissions were unchanged
    TEST_CHECK(orig_p == status(dir).permissions());

    // Test throwing version
    TEST_CHECK(fs::create_directory(dir, dir2) == false);
}

TEST_CASE(create_directory_one_level)
{
    scoped_test_env env;
    // Remove setgid which mkdir would inherit
    permissions(env.test_root, perms::remove_perms | perms::set_gid);

    const path dir = env.make_env_path("dir1");
    const path attr_dir = env.create_dir("dir2");
    permissions(attr_dir, perms::none);

    std::error_code ec;
    TEST_CHECK(fs::create_directory(dir, attr_dir, ec) == true);
    TEST_CHECK(!ec);
    TEST_CHECK(is_directory(dir));

    // Check that the new directory has the same permissions as attr_dir
    auto st = status(dir);
    TEST_CHECK(st.permissions() == perms::none);
}

TEST_CASE(create_directory_multi_level)
{
    scoped_test_env env;
    const path dir = env.make_env_path("dir1/dir2");
    const path dir1 = env.make_env_path("dir1");
    const path attr_dir = env.create_dir("attr_dir");
    std::error_code ec;
    TEST_CHECK(fs::create_directory(dir, attr_dir, ec) == false);
    TEST_CHECK(ec);
    TEST_CHECK(!is_directory(dir));
    TEST_CHECK(!is_directory(dir1));
}

TEST_CASE(dest_is_file)
{
    scoped_test_env env;
    const path file = env.create_file("file", 42);
    const path attr_dir = env.create_dir("attr_dir");
    std::error_code ec;
    TEST_CHECK(fs::create_directory(file, attr_dir, ec) == false);
    TEST_CHECK(ec);
    TEST_CHECK(is_regular_file(file));
}

TEST_SUITE_END()
