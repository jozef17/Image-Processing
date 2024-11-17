#include <gtest/gtest.h>
#include "Image.hpp"
#include "Pixel.hpp"

// Check whether proper width and height are returned
TEST(ImageTest, GetWidthAndHeight)
{
	Image image(64, 32);

	EXPECT_EQ(64, image.GetWidth());
	EXPECT_EQ(32, image.GetHeight());
}

// Check getting default (black) pixel
TEST(ImageTest, GetDefaultPixel)
{
	Image image(64, 32);
	auto pixel = image.GetPixel(8, 4);
	auto val = pixel.ToRGB();

	EXPECT_EQ(0, val.red);
	EXPECT_EQ(0, val.green);
	EXPECT_EQ(0, val.blue);
}

// Check setting and getting white pixel
TEST(ImageTest, SetAndGetWhite)
{
	Image image(16, 8);

	Pixel white = RGBPixel{ 255,255,255 };
	image.SetPixel(8, 4, white);
	auto pixel = image.GetPixel(8, 4);
	auto val = pixel.ToRGB();

	EXPECT_EQ(255, val.red);
	EXPECT_EQ(255, val.green);
	EXPECT_EQ(255, val.blue);
}

// Check setting and getting red pixel
TEST(ImageTest, SetAndGetRed)
{
	Image image(64, 32);

	Pixel red = RGBPixel{ 255,0,0 };
	image.SetPixel(32, 16, red);
	auto pixel = image.GetPixel(32, 16);
	auto val = pixel.ToRGB();

	EXPECT_EQ(255, val.red);
	EXPECT_EQ(0, val.green);
	EXPECT_EQ(0, val.blue);
}
