#pragma once

#ifndef IMAGE_HPP__
#define IMAGE_HPP__

#include <memory>

class Pixel;

class Image
{
public:
	Image(uint32_t width, uint32_t height);

	uint32_t GetWidth() const;
	uint32_t GetHeight() const;

	Pixel GetPixel(uint32_t x, uint32_t y) const;
	void  SetPixel(uint32_t x, uint32_t y, Pixel &p);

protected:
	Image();

	uint32_t width;
	uint32_t height;

	std::unique_ptr<std::unique_ptr<Pixel>[]> image;

};

#endif /* IMAGE_HPP__ */