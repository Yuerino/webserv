#include "gtest/gtest.h"
#include <vector>
#include <string>

#include "utils.hpp"
#include "Parser.hpp"

namespace webserv { namespace internal {

TEST(ParserTest, FullServerParseTest) {
	Parser parser;
	std::vector<ServerConfig> server_configs;

	ASSERT_NO_THROW(server_configs = parser.parse(file_to_string("test/config/parser_test_1.conf")));

	ASSERT_EQ(server_configs.size(), 1);
	ServerConfig server_config = server_configs.at(0);

	ASSERT_EQ(server_config.get_server_names().size(), 1);
	EXPECT_EQ(*server_config.get_server_names().begin(), "webserv");

	ASSERT_EQ(server_config.get_listens().size(), 1);
	EXPECT_EQ(server_config.get_listens().begin()->address, "0.0.0.0");
	EXPECT_EQ(server_config.get_listens().begin()->port, 8000);

	EXPECT_EQ(server_config.get_root(), "./server");
	EXPECT_EQ(server_config.get_index(), "index.php");

	ASSERT_EQ(server_config.get_allow_methods().size(), 1);
	EXPECT_EQ(*server_config.get_allow_methods().begin(), "GET");

	// Check location data
	ASSERT_EQ(server_config.get_locations().size(), 1);
	const LocationConfig location_config = server_config.get_locations().begin()->second;

	EXPECT_EQ(location_config.get_location(), "/put_test");
	EXPECT_EQ(location_config.get_root(), "./put_here");
	EXPECT_EQ(location_config.get_index(), "put_test.html");

	ASSERT_EQ(location_config.get_allow_methods().size(), 2);
	EXPECT_EQ(*location_config.get_allow_methods().begin(), "POST");
	EXPECT_EQ(*(++location_config.get_allow_methods().begin()), "PUT");

	EXPECT_EQ(location_config.get_cgi_path(), "script.js");
};

TEST(ParserTest, DefaultServerParseTest) {
	Parser parser;
	std::vector<ServerConfig> server_configs;

	ASSERT_NO_THROW(server_configs = parser.parse(file_to_string("test/config/parser_test_2.conf")));

	ASSERT_EQ(server_configs.size(), 1);
	ServerConfig server_config = server_configs.at(0);

	ASSERT_EQ(server_config.get_server_names().size(), 1);
	EXPECT_EQ(*server_config.get_server_names().begin(), "");

	ASSERT_EQ(server_config.get_listens().size(), 1);
	EXPECT_EQ(server_config.get_listens().begin()->address, "");
	EXPECT_EQ(server_config.get_listens().begin()->port, 80);

	EXPECT_EQ(server_config.get_root(), "html");
	EXPECT_EQ(server_config.get_index(), "index.html");

	ASSERT_EQ(server_config.get_allow_methods().size(), 8);
	EXPECT_TRUE(server_config.get_allow_methods().find("GET") != server_config.get_allow_methods().end());
	EXPECT_TRUE(server_config.get_allow_methods().find("HEAD") != server_config.get_allow_methods().end());
	EXPECT_TRUE(server_config.get_allow_methods().find("POST") != server_config.get_allow_methods().end());
	EXPECT_TRUE(server_config.get_allow_methods().find("PUT") != server_config.get_allow_methods().end());
	EXPECT_TRUE(server_config.get_allow_methods().find("DELETE") != server_config.get_allow_methods().end());
	EXPECT_TRUE(server_config.get_allow_methods().find("CONNECT") != server_config.get_allow_methods().end());
	EXPECT_TRUE(server_config.get_allow_methods().find("OPTIONS") != server_config.get_allow_methods().end());
	EXPECT_TRUE(server_config.get_allow_methods().find("TRACE") != server_config.get_allow_methods().end());

	// Check location data
	ASSERT_EQ(server_config.get_locations().size(), 1);
	const LocationConfig location_config = server_config.get_locations().begin()->second;

	EXPECT_EQ(location_config.get_location(), "/put_test");
	EXPECT_EQ(location_config.get_root(), "html");
	EXPECT_EQ(location_config.get_index(), "index.html");

	ASSERT_EQ(location_config.get_allow_methods().size(), 8);
	EXPECT_TRUE(location_config.get_allow_methods().find("GET") != location_config.get_allow_methods().end());
	EXPECT_TRUE(location_config.get_allow_methods().find("HEAD") != location_config.get_allow_methods().end());
	EXPECT_TRUE(location_config.get_allow_methods().find("POST") != location_config.get_allow_methods().end());
	EXPECT_TRUE(location_config.get_allow_methods().find("PUT") != location_config.get_allow_methods().end());
	EXPECT_TRUE(location_config.get_allow_methods().find("DELETE") != location_config.get_allow_methods().end());
	EXPECT_TRUE(location_config.get_allow_methods().find("CONNECT") != location_config.get_allow_methods().end());
	EXPECT_TRUE(location_config.get_allow_methods().find("OPTIONS") != location_config.get_allow_methods().end());
	EXPECT_TRUE(location_config.get_allow_methods().find("TRACE") != location_config.get_allow_methods().end());

	EXPECT_EQ(location_config.get_cgi_path(), "");
};

TEST(ParserTest, FailParseTest) {
	Parser parser;
	std::vector<ServerConfig> server_configs;

	EXPECT_ANY_THROW(server_configs = parser.parse(file_to_string("test/config/parser_test_3.conf")));
};

}} /* namespace webserv::internal */
