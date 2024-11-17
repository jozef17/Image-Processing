#include <gtest/gtest.h>
#include "png/PngLoader.hpp"
#include "Exception.hpp"
#include "Image.hpp"

#include <filesystem>
#include <fstream>

TEST(PngLoaderTest, IsPngFalse)
{
	uint8_t header[10] = { 0 };
	EXPECT_FALSE(PngLoader::IsPngImage(header, 10));
}

TEST(PngLoaderTest, IsPngTrue)
{

	uint8_t header[10] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0, 0 };
	EXPECT_TRUE(PngLoader::IsPngImage(header, 10));
}

TEST(PngLoaderTest, IsPngError)
{
	EXPECT_THROW(PngLoader::IsPngImage(nullptr, 0), RuntimeException);
}

TEST(PngLoaderTest, FileOpenError)
{
	auto path = std::filesystem::temp_directory_path().string() + "/not.a.file.321654.aaa.bbb_c";
	PngLoader loader(path);

	EXPECT_THROW(loader.LoadPngImage(), RuntimeException);
}