#include "gtest/gtest.h"

#include "LogData.hpp"

namespace webserv {

TEST(LogDataTest, LogDataInfoTest) {
	LogData data(LOG_INFO, __FILE__, __LINE__);
	data << "Some info: " << 42 << ", and another one\n";

	std::string without_time = data.get_message().substr(15);

	EXPECT_EQ(without_time, "\x1B[0m\x1B[32m[INFO] Some info: 42, and another one\n");
};

TEST(LogDataTest, LogDataDebugTest) {
	LogData data(LOG_DEBUG, __FILE__, __LINE__);
	data << "Some debug: " << 42 << ", and another one\n";

	std::string without_time = data.get_message().substr(15);

	EXPECT_EQ(without_time, "\x1B[0m\x1B[33m[DEBUG] [test/logger_test.cpp:17] Some debug: 42, and another one\n");
};

TEST(LogDataTest, LogDataErrorTest) {
	LogData data(LOG_ERROR, __FILE__, __LINE__);
	data << "Some error: " << 42 << ", and another one\n";

	std::string without_time = data.get_message().substr(15);

	EXPECT_EQ(without_time, "\x1B[0m\x1B[31m[ERROR] Some error: 42, and another one\n");
};

} /* namespace webserv */
