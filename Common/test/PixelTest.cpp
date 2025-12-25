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
	Pixel pixel = RGBPixel{ 71, 151, 98 };
	auto ycbcr = pixel.ToYCbCr();

	EXPECT_EQ(121, ycbcr.y);
	EXPECT_EQ(115, ycbcr.Cb);
	EXPECT_EQ(92, ycbcr.Cr);
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
	Pixel pixel = RGBAPixel{ 0, 3, 0, 128 };
	auto ycbcr = pixel.ToYCbCr();

	EXPECT_EQ(2, ycbcr.y);
	EXPECT_EQ(127, ycbcr.Cb);
	EXPECT_EQ(127, ycbcr.Cr);
}

TEST(PixelTest, YCbCr2RGB)
{
	Pixel pixel = YCbCrPixel{ 43, 122, 164 };
	auto rgb = pixel.ToRGB();

	EXPECT_EQ(93, rgb.red);
	EXPECT_EQ(19, rgb.green);
	EXPECT_EQ(32, rgb.blue);
}

TEST(PixelTest, YCbCr2RGBA)
{
	Pixel pixel = YCbCrPixel{ 218, 5, 155 };
	auto rgba = pixel.ToRGBA();

	EXPECT_EQ(255, rgba.red);
	EXPECT_EQ(241, rgba.green);
	EXPECT_EQ(0, rgba.blue);
	EXPECT_EQ(0, rgba.alpha);
}

// NOTE: Input and output values for test were generated from MATLAB
TEST(PixelTest, RGB2YCbCrMultiple)
{
	RGBPixel pixels[] = {
		{20, 0, 0}, {3, 0, 0}, {252, 241, 3}, {255, 255, 255}, {106, 15, 14},
		{66, 1, 0}, {205, 60, 77}, {255, 241, 0},{235, 28, 36}, {253, 255, 254}
	};

	YCbCrPixel reference[] = {
		{6, 125, 138}, {1, 127, 130}, {217, 7, 153}, {255, 128, 128}, {42, 112, 174},
		{20, 117, 161}, {105, 112, 199}, {218,5,155}, {91, 97, 231}, {254, 128, 127}
	};

	for (auto i = 0u; i < (sizeof(pixels) / sizeof(RGBPixel)); i++)
	{
		Pixel p = { pixels[i] };
		auto yCbCr = p.ToYCbCr();

		EXPECT_NEAR(yCbCr.y, reference[i].y, 1);
		EXPECT_NEAR(yCbCr.Cb, reference[i].Cb, 1);
		EXPECT_NEAR(yCbCr.Cr, reference[i].Cr, 1);
	}
}
