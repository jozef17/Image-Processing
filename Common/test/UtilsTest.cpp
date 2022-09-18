#include <gtest/gtest.h>
#include "Utils.hpp"

TEST(UtilsTest, EndsWith)
{
	EXPECT_TRUE(Utils::EndsWith("abcdefghi123", "123"));
	EXPECT_FALSE(Utils::EndsWith("abcdefghi12312", "123"));
	EXPECT_FALSE(Utils::EndsWith("123", "abcdefghi123"));
}

TEST(UtilsTest, ToLowercase)
{
	EXPECT_EQ(Utils::ToLowercase("AbCDeF"), "abcdef");
}

TEST(UtilsTest, ToUppercase)
{
	EXPECT_EQ(Utils::ToUppercase("AbCDeF"), "ABCDEF");
}

TEST(UtilsTest, IsNumber)
{
	EXPECT_EQ(Utils::IsNumber("AbCDeF"), false);
	EXPECT_EQ(Utils::IsNumber("123"), true);
	EXPECT_EQ(Utils::IsNumber("12.3"), true);
}
