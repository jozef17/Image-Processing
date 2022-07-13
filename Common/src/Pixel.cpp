#include "Pixel.hpp"

#include <cmath>

Pixel::Pixel() : Pixel(RGBPixel{ 0,0,0 }) {}

Pixel::Pixel(RGBPixel pixel) : type(Pixel::PixelType::RGB)
{
	this->pixel.rgbPixel = pixel;
}

Pixel::Pixel(RGBAPixel pixel) : type(Pixel::PixelType::RGBA)
{
	this->pixel.rgbaPixel = pixel;
}

Pixel::Pixel(YCbCrPixel pixel) : type(Pixel::PixelType::YCBCR)
{
	this->pixel.ycbcrPixel = pixel;
}

RGBPixel Pixel::ToRGB()
{
	if (this->type == Pixel::PixelType::RGB)
	{
		return this->pixel.rgbPixel;
	}
	else if (this->type == Pixel::PixelType::RGBA)
	{
		return RGBPixel{ this->pixel.rgbaPixel.red,
						 this->pixel.rgbaPixel.green,
						 this->pixel.rgbaPixel.blue };
	}

	auto red = 1.164 * (this->pixel.ycbcrPixel.y - 16.0) + 1.596 * (this->pixel.ycbcrPixel.Cr - 128.0);
	auto green = 1.164 * (this->pixel.ycbcrPixel.y - 16.0) - 0.813 * (this->pixel.ycbcrPixel.Cr - 128.0) - 0.392 * (this->pixel.ycbcrPixel.Cb - 128.0);
	auto blue = 1.164 * (this->pixel.ycbcrPixel.y - 16.0) + 2.017 * (this->pixel.ycbcrPixel.Cb - 128.0);
	return RGBPixel{ (uint8_t)std::round(red), (uint8_t)std::round(green), (uint8_t)std::round(blue) };
}

RGBAPixel Pixel::ToRGBA()
{
	if (this->type == Pixel::PixelType::RGB)
	{
		return RGBAPixel{ this->pixel.rgbPixel.red,
						  this->pixel.rgbPixel.green,
						  this->pixel.rgbPixel.blue,
						  0 };
	}
	else if (this->type == Pixel::PixelType::RGBA)
	{
		return this->pixel.rgbaPixel;
	}

	auto rgb = ToRGB();
	return RGBAPixel{ rgb.red, rgb.green, rgb.blue, 0 };
}

YCbCrPixel Pixel::ToYCbCr()
{
	if (this->type == Pixel::PixelType::YCBCR)
	{
		return this->pixel.ycbcrPixel;
	}

	auto rgb = ToRGB();
	auto y = 0.257 * rgb.red + 0.504 * rgb.green + 0.098 * rgb.blue + 16;
	auto cb = -0.148 * rgb.red - 0.291 * rgb.green + 0.439 * rgb.blue + 128;
	auto cr = 0.439 * rgb.red - 0.368 * rgb.green - 0.071 * rgb.blue + 128;

	return YCbCrPixel{ (uint8_t)std::round(y), (uint8_t)std::round(cb), (uint8_t)std::round(cr) };
}
