#include "gtest/gtest.h"
#include <vector>
#include <string>
#include <cstdlib>

#include "utils.hpp"

namespace webserv { namespace internal {

TEST(UtilsTest, ToStringTest) {
	EXPECT_EQ(to_string(42), "42");
	EXPECT_EQ(to_string(42.69f), "42.69");
};

TEST(UtilsTest, FileToStringTest) {
	EXPECT_EQ(file_to_string("test/config/parser_test_2.conf"), "server {\n\tlocation /put_test {\n\t}\n}\n");
};

TEST(UtilsTest, IsValidExtensionTest) {
	EXPECT_TRUE(is_valid_extension("test/config/parser_test_1.conf", ".conf"));
	EXPECT_TRUE(is_valid_extension("utils_test.cpp", ".cpp"));
	EXPECT_FALSE(is_valid_extension("utils_test.cpp", ".conf"));
};

TEST(UtilsTest, IsDigitsTest) {
	EXPECT_TRUE(is_digits("42"));
	EXPECT_TRUE(is_digits("-6942"));
	EXPECT_FALSE(is_digits("42abc"));
	EXPECT_FALSE(is_digits("42.5f"));
};

}} /* namespace webserv::internal */
