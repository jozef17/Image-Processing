#include "gif/GifLoader.hpp"
#include "gif/LZW.hpp"
#include "BitStream.hpp"
#include "Image.hpp"
#include "Exception.hpp"

#include <fstream>
#include <map>

GifLoader::GifLoader(const std::string& filename) : filename(filename) {}

bool GifLoader::IsGifImage(uint8_t* header, uint32_t size)
{
	if (size < 6)
	{
		throw RuntimeException("Not enough data to asses the file (gif)");
	}

	if (header[0] != 0x47 || header[1] != 0x49 || header[2] != 0x46 || header[3] != 0x38
		|| !(header[4] == 0x37 || header[4] == 0x39) || header[5] != 0x61)
	{
		return false;
	}
	return true;
}

std::unique_ptr<Image> GifLoader::LoadGifImage()
{
	std::ifstream file(this->filename, std::ios::binary);
	if (!file.is_open())
	{
		throw RuntimeException("Unable to open file: \"" + filename + "\"");
	}

	// Check header
	uint8_t header[6];
	file.read((char*)&header, 6);
	if (!IsGifImage(header, 6))
	{
		throw RuntimeException("Not a gif file (" + filename + ")!");
	}

	// Logical Screen Descriptor & Global color Table
	uint8_t screenDescriptor[7];
	file.read((char*)&screenDescriptor, 7);
	this->isGlobalColorTablePresent = screenDescriptor[4] & 1 << 7; // bit 7 of 4th byte is global color table flag
	if (this->isGlobalColorTablePresent)
	{
		LoadColorTable(file, screenDescriptor[4]);
	}

	// Graphics Control Extension
	uint8_t nextBlock[2];
	file.read((char*)&nextBlock, 2);
	if (nextBlock[0] != 0x21 || nextBlock[1] != 0xF9)
	{
		throw RuntimeException("Missing Graphics Control Extension!");
	}
	else
	{
		uint8_t graphicsColorExtensions[6];
		file.read((char*)&graphicsColorExtensions, 6);
		uint8_t transparrentColorIndex = graphicsColorExtensions[4];
	}

	// Image Descriptor
	uint8_t imageDescriptor[10];
	file.read((char*)&imageDescriptor, 10);
	if (imageDescriptor[0] != 0x2C)
	{
		throw RuntimeException("Image Descriptor Missing!");
	}

	this->width = ((int)imageDescriptor[6] << 8) | imageDescriptor[5];
	this->height = ((int)imageDescriptor[8] << 8) | imageDescriptor[7];

	bool isLocalColorTablePresent = imageDescriptor[9] & 1 << 7;
	if (this->isGlobalColorTablePresent && isLocalColorTablePresent)
	{
		throw RuntimeException("Only one color table supported!");
	}
	else if (isLocalColorTablePresent)
	{
		LoadColorTable(file, imageDescriptor[9]);
	}

	// Load Image Data
	BitStream stream;
	uint8_t lzwMinCodeSize;
	file.read((char*)&lzwMinCodeSize, 1);
	while (true)
	{
		std::unique_ptr<uint8_t[]> data;
		uint8_t imageDatasize = (uint8_t)-1;

		file.read((char*)&imageDatasize, 1);
		if (imageDatasize == 0) // Size 0 = End of data
		{
			break;
		}

		if (!file.good())
		{
			throw RuntimeException("Unexpectadly reached end of file!");
		}

		data = std::unique_ptr<uint8_t[]>(new uint8_t[imageDatasize]);
		file.read((char*)data.get(), imageDatasize);
		stream.Append(std::move(data), imageDatasize);
	}

	// Decode data
	LZW lzw(stream, lzwMinCodeSize, (uint16_t)this->colorTable.size());
	auto imageColorIndexes = lzw.Decode();
	return ProcessImageData(imageColorIndexes);
}

void GifLoader::LoadColorTable(std::ifstream& file, uint8_t flags)
{
	// For now support only 8bits colors
	if (this->isGlobalColorTablePresent && (flags & 0b01110000) != 0b01110000)
	{
		throw RuntimeException("Unsupported color resolution!");
	}

	// Get number of colors in global color table
	int n = flags & 0b00000111;
	auto colorCount = 1 << (n + 1);

	RGBPixel pixel;
	for (int i = 0; i < colorCount; i++)
	{
		file.read((char*)&pixel, 3);
		this->colorTable[i] = pixel;
	}
}

std::unique_ptr<Image> GifLoader::ProcessImageData(std::vector<uint8_t> imageColorIndexes)
{
	if (imageColorIndexes.size() != width * height)
	{
		throw Exception("LZW decoding failed!");
	}

	// Build image
	std::unique_ptr<Image> image = std::make_unique<Image>(width, height);
	uint32_t byteCounter = 0;
	for (uint32_t y = 0; y < this->height; y++)
	{
		for (uint32_t x = 0; x < this->width; x++)
		{
			auto index = imageColorIndexes[byteCounter++];
			auto color = this->colorTable[index];
			image->SetPixel(x, y, Pixel(color));
		}
	}
	return image;
}
