#include "RAWLoader.hpp"
#include "Pixel.hpp"
#include "Exception.hpp"

#include <fstream>

RAWLoader::RAWLoader(std::string filename, uint32_t width, uint32_t height)
	: filename(filename), width(width), height(height) {}

std::unique_ptr<Image> RAWLoader::LoadRawImage()
{
	std::unique_ptr<Image> img = std::make_unique<Image>(this->width, this->height);

	// Check file
	std::ifstream file(filename);
	if (!file.is_open())
	{
		throw RuntimeException("Unable to open file: \"" + filename + "\"");
	}

	// Read file
	for (uint32_t y = 0u; y < this->height; y++)
	{
		for (uint32_t x = 0u; x < this->width; x++)
		{
			RGBPixel rgb;
			file.read((char*)&rgb, 3);
			img->SetPixel(x,y, Pixel(rgb));
		}
	}

	file.close();
	return img;
}
