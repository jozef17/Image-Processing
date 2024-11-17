#include <gtest/gtest.h>
#include "RAWLoader.hpp"
#include "Exception.hpp"

#include <filesystem>
#include <fstream>

TEST(RAWLoaderTest, OpenError)
{
	auto tmp = std::filesystem::temp_directory_path().string();
	RAWLoader loader(tmp + "/not.a.file.321654.aaa.bbb_c", 100, 100);
	EXPECT_THROW(loader.LoadRawImage(), RuntimeException);
}

TEST(RAWLoaderTest, HappyPath1x1)
{
	auto tmp = std::filesystem::temp_directory_path().string();
	auto filename = tmp + "/test_file_123.raw";

	// Prepare test file
	{
		std::ofstream testFile(filename, std::ios::binary);
		uint8_t byte = 128;
		testFile << byte << byte << byte;
	}

	// Load Image
	RAWLoader loader(filename, 1, 1);
	auto result = loader.LoadRawImage();

	// Check dimmensions
	EXPECT_EQ(result->GetHeight(), 1);
	EXPECT_EQ(result->GetWidth(), 1);

	auto pixel = result->GetPixel(0, 0).ToRGB();

	// Check data
	EXPECT_EQ(pixel.red, 128);
	EXPECT_EQ(pixel.green, 128);
	EXPECT_EQ(pixel.blue, 128);

	// Cleanup
	std::filesystem::remove(filename);
}
