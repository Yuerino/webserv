#include "gtest/gtest.h"

#include "Logger.hpp"

namespace webserv::internal {

TEST(LogDataTest, LogDataInfoTest) {
	LogData data(LOG_INFO, __FILE__, __LINE__);
	data << "Some info: " << 42 << ", and another one\n";

	std::string without_time = data.get_message().substr(15);

	EXPECT_EQ(without_time, "\x1B[0m\x1B[32m[INFO] Some info: 42, and another one\n");
};

TEST(LogDataTest, LogDataErrorTest) {
	LogData data(LOG_ERROR, __FILE__, __LINE__);
	data << "Some error: " << 42 << ", and another one\n";

	std::string without_time = data.get_message().substr(15);

	EXPECT_EQ(without_time, "\x1B[0m\x1B[31m[ERROR] Some error: 42, and another one\n");
};

TEST(LogDataTest, LogDataDebugTest) {
	LogData data(LOG_DEBUG, __FILE__, __LINE__);
	data << "Some debug: " << 42 << ", and another one\n";

	std::string without_time = data.get_message().substr(15);

	EXPECT_EQ(without_time, "\x1B[0m\x1B[33m[DEBUG] [test/logger_test.cpp:26] Some debug: 42, and another one\n");
};

TEST(LoggerTest, SingletonTest) {
	const Logger& logger1 = Logger::get_instance();
	const Logger& logger2 = Logger::get_instance();

	EXPECT_EQ(&logger1, &logger2);
};

TEST(LoggerTest, LogInfoTest) {
	::testing::internal::CaptureStdout();

	LOG_I() << "Some info: " << 42 << ", and another one\n";

	std::string without_time = ::testing::internal::GetCapturedStdout().substr(15);

	EXPECT_EQ(without_time, "\x1B[0m\x1B[32m[INFO] Some info: 42, and another one\n\x1B[0m");
};

TEST(LoggerTest, LogErrorTest) {
	::testing::internal::CaptureStderr();

	LOG_E() << "Some error: " << 42 << ", and another one\n";

	std::string without_time = ::testing::internal::GetCapturedStderr().substr(15);

	EXPECT_EQ(without_time, "\x1B[0m\x1B[31m[ERROR] Some error: 42, and another one\n\x1B[0m");
};

TEST(LoggerTest, LogDebugTest) {
	::testing::internal::CaptureStderr();

	LOG_D() << "Some debug: " << 42 << ", and another one\n";

	std::string without_time = ::testing::internal::GetCapturedStderr().substr(15);

	EXPECT_EQ(without_time, "\x1B[0m\x1B[33m[DEBUG] [test/logger_test.cpp:64] Some debug: 42, and another one\n\x1B[0m");
};

} /* namespace webserv */
