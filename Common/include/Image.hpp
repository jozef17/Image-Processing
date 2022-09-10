#pragma once

#ifndef IMAGE_HPP__
#define IMAGE_HPP__

#include <memory>

#include "Pixel.hpp"

class Image
{
public:
	enum class StartPosition : uint8_t { TopLeft, BottomLeft };

	Image(uint32_t width, uint32_t height, StartPosition startPosition = StartPosition::TopLeft);
	virtual ~Image() = default;

	uint32_t GetWidth() const;
	uint32_t GetHeight() const;

	Pixel GetPixel(uint32_t x, uint32_t y) const;
	void  SetPixel(uint32_t x, uint32_t y, Pixel &p);

	inline StartPosition GetStartPosition() const noexcept { return this->startPosition; };

protected:
	Image() = default;

	StartPosition startPosition = StartPosition::TopLeft;

	std::unique_ptr<std::unique_ptr<Pixel>[]> image;

	uint32_t width  = 0;
	uint32_t height = 0;

};

#endif /* IMAGE_HPP__ */