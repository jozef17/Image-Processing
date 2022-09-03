#include <gtest/gtest.h>
#include "Utils.hpp"

TEST(UtilsTest, EndsWith)
{
	EXPECT_TRUE(Utils::EndsWith("abcdefghi123", "123"));
	EXPECT_FALSE(Utils::EndsWith("abcdefghi12312", "123"));
	EXPECT_FALSE(Utils::EndsWith("123", "abcdefghi123"));
}
