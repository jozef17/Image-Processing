#include "Pixel.hpp"
#include "Image.hpp"
#include "Exception.hpp"

Image::Image(uint32_t width, uint32_t height, StartPosition startPosition)
	: width(width), height(height), startPosition(startPosition)
{
	this->image = std::unique_ptr<std::unique_ptr<Pixel>[]>(new std::unique_ptr<Pixel>[width * height]);
	for(unsigned int i = 0; i < width * height; i++)
	{
		this->image[i] = std::unique_ptr<Pixel>(new Pixel);
	}
}

Image::~Image() 
{}

Image::Image() : width(0), height(0) 
{}

uint32_t Image::GetWidth() const
{
	return this->width;
}

uint32_t Image::GetHeight() const
{
	return this->height;
}

Pixel Image::GetPixel(uint32_t x, uint32_t y) const
{
	if (x > this->width || y > this->height)
	{
		std::string errorMessage = "Error: GetPixel - Attempt to request invalid pixel at [" + std::to_string(x) + ", " + std::to_string(y) +
			"] while image size is [" + std::to_string(this->width) + ", " + std::to_string(this->height) + "]";
		throw Exception(errorMessage.c_str());
	}

	return *(this->image[y * this->width + x]);
}

void Image::SetPixel(uint32_t x, uint32_t y, Pixel& p)
{
	if (x > this->width || y > this->height)
	{
		std::string errorMessage = "Error: GetPixel - Attempt to request invalid pixel at [" + std::to_string(x) + ", " + std::to_string(y) +
			"] while image size is [" + std::to_string(this->width) + ", " + std::to_string(this->height) + "]";
		throw Exception(errorMessage.c_str());
	}

	this->image[y * this->width + x] = std::make_unique<Pixel>(p);
}
