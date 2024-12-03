#include "ImageLoader.hpp"
#include "Image.hpp"
#include "RAWLoader.hpp"
#include "BitmapLoader.hpp"
#include "gif/GifLoader.hpp"
#include "png/PngLoader.hpp"
#include "Exception.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>

std::unique_ptr<Image> ImageLoader::LoadImage(const std::string& image)
{
	uint8_t buffer[16];
	{
		std::ifstream file(image, std::ios::binary);
		if (!file.good())
		{
			throw RuntimeException("Unable to open file: " + image + "!");
		}
		file.read((char*)buffer, sizeof(buffer));
	}

	if (PngLoader::IsPngImage(buffer, sizeof(buffer)))
	{
		PngLoader loader(image);
		return loader.LoadPngImage();
	}

	if (GifLoader::IsGifImage(buffer, sizeof(buffer)))
	{
		GifLoader loader(image);
		return loader.LoadGifImage();
	}

	if (BitmapLoader::IsBitmapImage(buffer, sizeof(buffer)))
	{
		BitmapLoader loader(image);
		return loader.LoadBitmapImage();
	}

	throw RuntimeException("Unsupported image format!");
}

std::unique_ptr<Image> ImageLoader::LoadRawImage(const std::string& image, uint32_t width, uint32_t height)
{
	RAWLoader loader(image, width, height);
	return loader.LoadRawImage();
}
