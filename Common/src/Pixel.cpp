#include "Pixel.hpp"

#include <cmath>
#include <algorithm>

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

	auto red = this->pixel.ycbcrPixel.y + 1.402 * (this->pixel.ycbcrPixel.Cr - 128.0);
	auto green = this->pixel.ycbcrPixel.y - 0.344136 * (this->pixel.ycbcrPixel.Cb - 128.0) - 0.714136 * (this->pixel.ycbcrPixel.Cr - 128.0);
	auto blue = this->pixel.ycbcrPixel.y + 1.772 * (this->pixel.ycbcrPixel.Cb - 128.0);

	red = std::clamp(std::round(red), 0.0, 255.0);
	green = std::clamp(std::round(green), 0.0, 255.0);
	blue = std::clamp(std::round(blue), 0.0, 255.0);

	return RGBPixel{ static_cast<uint8_t>(red), static_cast<uint8_t>(green), static_cast<uint8_t>(blue) };
} 

RGBAPixel Pixel::ToRGBA()
{
	if (this->type == Pixel::PixelType::RGB)
	{
		return RGBAPixel{ this->pixel.rgbPixel.red,
						  this->pixel.rgbPixel.green,
						  this->pixel.rgbPixel.blue,
						  255 };
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
	auto y = 0.299 * rgb.red + 0.587 * rgb.green + 0.114 * rgb.blue;	
	auto cb = 128.0 - 0.168736 * rgb.red - 0.331264 * rgb.green + 0.5 * rgb.blue;
	auto cr = 128.0 + 0.5 * rgb.red - 0.418688 * rgb.green - 0.081312 * rgb.blue;

	y = std::clamp(std::round(y), 0.0, 255.0);
	cb = std::clamp(std::round(cb), 0.0, 255.0);
	cr = std::clamp(std::round(cr), 0.0, 255.0);

	return YCbCrPixel{ static_cast<uint8_t>(y), static_cast<uint8_t>(cb), static_cast<uint8_t>(cr) };
}
