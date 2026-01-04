#include "Image.hpp"
#include "Exception.hpp"

Image::Image(uint32_t width, uint32_t height, StartPosition startPosition)
	: width(width), height(height), startPosition(startPosition), image(width * height)
{}

Pixel Image::GetPixel(uint32_t x, uint32_t y) const
{
	if (x > this->width || y > this->height)
	{
		std::string errorMessage = "Error: GetPixel - Attempt to request invalid pixel at [" + std::to_string(x) + ", " + std::to_string(y) +
			"] while image size is [" + std::to_string(this->width) + ", " + std::to_string(this->height) + "]";
		throw Exception(errorMessage.c_str());
	}

	return this->image[y * this->width + x];
}

void Image::SetPixel(uint32_t x, uint32_t y, const Pixel& p)
{
	if (x > this->width || y > this->height)
	{
		std::string errorMessage = "Error: GetPixel - Attempt to request invalid pixel at [" + std::to_string(x) + ", " + std::to_string(y) +
			"] while image size is [" + std::to_string(this->width) + ", " + std::to_string(this->height) + "]";
		throw Exception(errorMessage.c_str());
	}

	if (this->startPosition == Image::StartPosition::BottomLeft)
	{
		y = this->height - y - 1;
	}

	this->image[y * this->width + x] = p;
}
