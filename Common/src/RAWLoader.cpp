#include "RAWImage.hpp"
#include "Pixel.hpp"
#include "Exception.hpp"

#include <fstream>

RAWImage::RAWImage(std::string filename, uint32_t width, uint32_t height)
{
	this->width = width;
	this->height = height;

	LoadData(filename);
}

void RAWImage::LoadData(std::string filename)
{
	// Check file
	std::ifstream file(filename);
	if (!file.is_open())
	{
		throw RuntimeException("Unable to open file: \"" + filename + "\"");
	}

	// Read file
	this->image = std::unique_ptr<std::unique_ptr<Pixel>[]>(new std::unique_ptr<Pixel>[width * height]);
	for (uint32_t i = 0; i < this->width * this->height; i++)
	{
		RGBPixel rgb;
		file.read((char*)&rgb, 3);
		this->image[i] = std::unique_ptr<Pixel>(new Pixel(rgb));
	}

	file.close();
}
