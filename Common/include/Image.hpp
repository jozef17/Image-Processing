#pragma once

#ifndef IMAGE_HPP__
#define IMAGE_HPP__

#include <vector>

#include "Pixel.hpp"

class Image
{
public:
	enum class StartPosition : uint8_t { TopLeft, BottomLeft };

	Image(uint32_t width, uint32_t height, StartPosition startPosition = StartPosition::TopLeft);
	virtual ~Image() = default;

	inline auto GetWidth() const { return this->width; };
	inline auto GetHeight() const { return this->height; };

	Pixel GetPixel(uint32_t x, uint32_t y) const;
	void  SetPixel(uint32_t x, uint32_t y, Pixel &p);

protected:
	Image() = default;

	StartPosition startPosition = StartPosition::TopLeft;

	std::vector<Pixel> image;

	uint32_t width  = 0;
	uint32_t height = 0;
};

#endif /* IMAGE_HPP__ */