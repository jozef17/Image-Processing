#pragma once

#ifndef PIXEL_HPP__
#define PIXEL_HPP__

#include <cstdint>

struct RGBPixel
{
	uint8_t red;
	uint8_t green;
	uint8_t blue;
};

struct RGBAPixel
{
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t alpha;
};

struct YCbCrPixel
{
	union
	{
		struct
		{
			uint8_t luma;
			uint8_t blueChroma;
			uint8_t redChroma;
		};
		struct
		{
			uint8_t y;
			uint8_t Cb;
			uint8_t Cr;
		};
	};
};

class Pixel
{
public:
	Pixel();
	Pixel(RGBPixel pixel);
	Pixel(RGBAPixel pixel);
	Pixel(YCbCrPixel pixel);

	RGBPixel ToRGB();
	RGBAPixel ToRGBA();
	YCbCrPixel ToYCbCr();

private:
	enum class PixelType : uint8_t { RGB, RGBA, YCBCR };

	union PixelInternal
	{
		RGBPixel rgbPixel;
		RGBAPixel rgbaPixel;
		YCbCrPixel ycbcrPixel;
	};

	const PixelType type;
	PixelInternal pixel;
};


#endif /* PIXEL_HPP__ */
