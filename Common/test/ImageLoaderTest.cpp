#include <gtest/gtest.h>
#include "ImageLoader.hpp"
#include "Exception.hpp"
#include "Image.hpp"

#include <filesystem>

TEST(ImageLoaderTest, FileOpenError)
{
	auto tmp = std::filesystem::temp_directory_path().string();
	EXPECT_THROW(ImageLoader::LoadImage(tmp + "/not.a.file.321654.aaa.bbb_c"), RuntimeException);
	EXPECT_THROW(ImageLoader::LoadRawImage(tmp + "/not.a.file.321654.aaa.bbb_c", 100, 100), RuntimeException);
}
