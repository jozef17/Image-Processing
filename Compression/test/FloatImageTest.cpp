#include <gtest/gtest.h>
#include "FloatImage.hpp"
#include "Image.hpp"
#include "Pixel.hpp"



TEST(FloatImageTest, Constructor)
{
	FloatImage image(128, 256);

	EXPECT_EQ(image.GetWidth(),  128);
	EXPECT_EQ(image.GetHeight(), 256);
}

TEST(FloatImageTest, CreateFromImage)
{
	Image image(64, 32);
	YCbCrPixel pixel = { 0,128,255 };
	image.SetPixel(10, 10, Pixel{ pixel });

	FloatImage fimg(image);

	EXPECT_EQ(fimg.GetWidth(), 64);
	EXPECT_EQ(fimg.GetHeight(), 32);

	auto data = fimg.Get(10, 10);
	EXPECT_EQ(data.data[0], 0);
	EXPECT_EQ(data.data[1], 128);
	EXPECT_EQ(data.data[2], 255);
}

TEST(FloatImageTest, ToImage)
{
	FloatImage fimg(32, 64);
	fimg.Set(31, 63, { 123.f, 25.22f, 33.75f });

	auto image = fimg.GetImage();

	EXPECT_EQ(image->GetWidth(), 32);
	EXPECT_EQ(image->GetHeight(), 64);

	auto pixel = image->GetPixel(31, 63).ToYCbCr();
	EXPECT_EQ(pixel.y, 123);
	EXPECT_EQ(pixel.Cb, 25);
	EXPECT_EQ(pixel.Cr, 33);
}

TEST(FloatImageTest, SetAndGetFloatArray)
{
	FloatImage image(128, 256);
	float data[3] = { -500, 100, 12.5 };
	image.Set(0, 0, data);

	auto &result = image.Get(0, 0);
	EXPECT_EQ(result.data[0], -500);
	EXPECT_EQ(result.data[1], 100);
	EXPECT_EQ(result.data[2], 12.5);
}

TEST(FloatImageTest, SetAndGetFloatData)
{
	FloatImage image(128, 256);
	FloatData data = { -500, 100, 12.5};
	image.Set(0, 0, data);

	auto &result = image.Get(0, 0);
	EXPECT_EQ(result.data[0], -500);
	EXPECT_EQ(result.data[1], 100);
	EXPECT_EQ(result.data[2], 12.5);
}
