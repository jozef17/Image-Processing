#include <gtest/gtest.h>
#include "BitmapLoader.hpp"
#include "Exception.hpp"
#include "Image.hpp"

#include <filesystem>
#include <fstream>

TEST(BitmapLoaderTest, IsBitmapFalse)
{
	uint8_t header[4] = { 0 };
	EXPECT_FALSE(BitmapLoader::IsBitmapImage(header, 4));
}

TEST(BitmapLoaderTest, IsBitmapTrue)
{
	uint8_t header[4] = { 0x42, 0x4D, 0, 0 };
	EXPECT_TRUE(BitmapLoader::IsBitmapImage(header, 4));
}

TEST(BitmapLoaderTest, IsBitmapError)
{
	EXPECT_THROW(BitmapLoader::IsBitmapImage(nullptr, 0), RuntimeException);
}

TEST(BitmapLoaderTest, FileOpenError)
{
	auto path = std::filesystem::temp_directory_path().string() + "/not.a.file.321654.aaa.bbb_c";
	BitmapLoader loader(path);

	EXPECT_THROW(loader.LoadBitmapImage(), RuntimeException);
}
