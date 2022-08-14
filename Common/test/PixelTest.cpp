#include <gtest/gtest.h>
#include "Pixel.hpp"


TEST(PixelTest, RGB2RGBA)
{
	Pixel pixel = RGBPixel{ 1, 128, 255 };	
	auto rgba = pixel.ToRGBA();

	EXPECT_EQ(1, rgba.red);
	EXPECT_EQ(128, rgba.green);
	EXPECT_EQ(255, rgba.blue);
	EXPECT_EQ(255, rgba.alpha);
}

TEST(PixelTest, RGB2YCbCr)
{
	Pixel pixel = RGBPixel{ 105, 128, 172 };
	auto ycbcr = pixel.ToYCbCr();

	EXPECT_EQ(124, ycbcr.y);
	EXPECT_EQ(151, ycbcr.Cb);
	EXPECT_EQ(115, ycbcr.Cr);
}

TEST(PixelTest, RGBA2RGB)
{
	Pixel pixel = RGBAPixel{ 1, 128, 255, 128 };
	auto rgb = pixel.ToRGB();

	EXPECT_EQ(1, rgb.red);
	EXPECT_EQ(128, rgb.green);
	EXPECT_EQ(255, rgb.blue);
}

TEST(PixelTest, RGBA2YCbCr)
{
	Pixel pixel = RGBAPixel{ 129, 151, 190, 128 };
	auto ycbcr = pixel.ToYCbCr();

	EXPECT_EQ(144, ycbcr.y);
	EXPECT_EQ(148, ycbcr.Cb);
	EXPECT_EQ(116, ycbcr.Cr);
}

TEST(PixelTest, YCbCr2RGB)
{
	Pixel pixel = YCbCrPixel{ 143, 151, 115 };
	auto rgb = pixel.ToRGB();

	EXPECT_EQ(127, rgb.red);
	EXPECT_EQ(149, rgb.green);
	EXPECT_EQ(194, rgb.blue);
}

TEST(PixelTest, YCbCr2RGBA)
{
	Pixel pixel = YCbCrPixel{ 52, 118, 132 };
	auto rgba = pixel.ToRGBA();

	EXPECT_EQ(48, rgba.red);
	EXPECT_EQ(43, rgba.green);
	EXPECT_EQ(22, rgba.blue);
	EXPECT_EQ(0, rgba.alpha);
}
