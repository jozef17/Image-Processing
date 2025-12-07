#include <gtest/gtest.h>
#include "jpg/JpegLoader.hpp"
#include "Exception.hpp"

TEST(JpegLoaderTest, IsJpegFalse)
{
	uint8_t header[10] = { 0 };
	EXPECT_FALSE(JpegLoader::IsJpegImage(header, 10));
}

TEST(JpegLoaderTest, IsJpegTrue)
{
	uint8_t header[4] = { 0xff, 0xd8, 0xff, 0xe0 };
	EXPECT_TRUE(JpegLoader::IsJpegImage(header, 4));
}

TEST(JpegLoaderTest, IsJpegError)
{
	EXPECT_THROW(JpegLoader::IsJpegImage(nullptr, 0), RuntimeException);
}
