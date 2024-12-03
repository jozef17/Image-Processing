#include "GifLoader.hpp"
#include "BitStream.hpp"
#include "Image.hpp"
#include "Exception.hpp"

#include <fstream>
#include <map>

#ifdef ENABLE_LOGS
#include <iostream>
#include <iomanip>
#endif

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

	uint32_t width = ((int)imageDescriptor[6] << 8) | imageDescriptor[5];
	uint32_t height = ((int)imageDescriptor[8] << 8) | imageDescriptor[7];
#ifdef ENABLE_LOGS
	std::cout << "Width:  " << (int)imageDescriptor[5] << " " <<
		std::setw(2) << std::setfill('0') << std::hex << (int)imageDescriptor[6] << " = " << std::dec << width << std::endl;
	std::cout << "Height: " << std::setw(2) << std::setfill('0') << std::hex << (int)imageDescriptor[7] << " " <<
		std::setw(2) << std::setfill('0') << std::hex << (int)imageDescriptor[8] << " = " << std::dec << height << std::endl;
	std::cout << "Flags: ";
	for (int i = 7; i >= 0; i--)
		std::cout << ((imageDescriptor[9] & (1 << i)) ? "1" : "0");
	std::cout << std::endl;
#endif

	bool isLocalColorTablePresent = imageDescriptor[9] & 1 << 7;
	if (this->isGlobalColorTablePresent && isLocalColorTablePresent)
	{
		throw RuntimeException("Only one color table supported!");
	}
	
	if (isLocalColorTablePresent)
	{
#ifdef ENABLE_LOGS
		std::cout << "Using local colot table!!!" << std::endl;
#endif
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
#ifdef ENABLE_LOGS
		std::cout << "Image data size:  " << std::dec << (int)imageDatasize << std::endl;
		for (int i = 0; i < imageDatasize; i++)
			std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)data[i] << " ";
		std::cout << std::endl;
#endif
		stream.Append(std::move(data), imageDatasize);
	}

	// Decode data
	std::vector<uint8_t> imageColorIndexes = LZWDecode(stream, lzwMinCodeSize);
#ifdef ENABLE_LOGS
	std::cout << "Size: " << imageColorIndexes.size() << ": ";
	for (auto x : imageColorIndexes)
		std::cout << std::dec << (int)x << " ";
#endif
	if (imageColorIndexes.size() != width * height)
	{
		throw Exception("LZW decoding failed!");
	}

	// Build image
	std::unique_ptr<Image> image = std::make_unique<Image>(width, height);
	uint32_t byteCounter = 0;
	for (uint32_t y = 0; y < height; y++)
	{
		for (uint32_t x = 0; x < width; x++)
		{
			auto index = imageColorIndexes[byteCounter++];
			auto color = this->colorTable[index];
			image->SetPixel(x, y, Pixel(color));
		}
	}
	return image;
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

#ifdef ENABLE_LOGS
	std::cout << " (" << std::dec << n << ") colorCount: " << colorCount << std::endl;
#endif
	RGBPixel pixel;
	for (int i = 0; i < colorCount; i++)
	{
		file.read((char*)&pixel, 3);
		this->colorTable[i] = pixel;
#ifdef ENABLE_LOGS
		std::cout << std::dec << "Color " << i << ": " << std::setw(2) << std::setfill('0') << std::hex << i << " " << (int)pixel.red << " " << (int)pixel.green << " " << (int)pixel.blue << std::endl;
#endif
	}
}

std::vector<uint8_t> GifLoader::LZWDecode(BitStream& stream, uint8_t lzwMinCodeSize)
{
	std::map<uint32_t, std::vector<uint32_t>> decodeTable;
	std::vector<uint8_t> imageColorIndexes;
	uint32_t nextCodeIndex = 0;
	uint32_t codeSize = lzwMinCodeSize;
	uint32_t maxCode = (1 << (codeSize + 1));

	// Decode
#ifdef ENABLE_LOGS
	std::cout << "Decoding:" << std::endl;
#endif
	uint32_t prewCode = (uint32_t)-1;
	while (true)
	{
		uint32_t code = 0;
		for (uint32_t i = 0; i <= codeSize; i++)
		{
			uint32_t bit = stream.GetNext() ? 1 : 0;
			code = code | (bit << i);
		}

		// End of data!
		if (code == (this->colorTable.size() + 1))
		{
			break;
		}

		// Clear / reset
		if (code == (this->colorTable.size()))
		{
#ifdef ENABLE_LOGS
			std::cout << "Reset code" << std::endl;
#endif
			decodeTable.clear();

			// Generate Decode table
			for (uint32_t i = 0; i < this->colorTable.size(); i++)
			{
				decodeTable[i] = { i }; // Clear / Reset code
			}
			nextCodeIndex = (uint32_t)this->colorTable.size() + 2;
			prewCode = (uint32_t)-1;
			codeSize = lzwMinCodeSize;
			maxCode = (1 << (codeSize + 1));

			continue;
		}

		// Output first color after clear
		if (prewCode == (uint32_t)-1)
		{
			imageColorIndexes.push_back(code);
			prewCode = code;
			continue;
		}
#ifdef ENABLE_LOGS
		std::cout << std::setw(4) << std::setfill('0') << std::hex << code << " ";
		std::cout << std::dec << code << " | ";
#endif
		if (decodeTable.find(code) != decodeTable.end())
		{
			// Code in table
			std::vector<uint32_t> pattern = decodeTable[code];
			imageColorIndexes.insert(imageColorIndexes.end(), pattern.begin(), pattern.end());

			// Add code to table
			auto first = pattern.at(0); // K
			std::vector<uint32_t> newPattern = decodeTable[prewCode];
			newPattern.push_back(first);
			decodeTable[nextCodeIndex++] = newPattern;
#ifdef ENABLE_LOGS
			std::cout << "Old code | addoing code " << (nextCodeIndex - 1) << " |";
			std::cout << "first(K) " << first << " (";
			for (auto x : newPattern)
				std::cout << (int)x << " ";
			std::cout << ")" << std::endl;
#endif
		}
		else if(code != nextCodeIndex)
		{
			throw("Invaid code " + std::to_string(code) + " (" + std::to_string(nextCodeIndex) + ")!!!");
		}
		else
		{
			// Code not in table;
			auto firstPrew = decodeTable[prewCode].at(0); // K
			std::vector<uint32_t> newPattern = decodeTable[prewCode];
			newPattern.push_back(firstPrew);
			decodeTable[nextCodeIndex++] = newPattern;
#ifdef ENABLE_LOGS
			std::cout << "New code | addoing code " << (nextCodeIndex - 1) << " |";;
			std::cout << "firstPrew (K) " << firstPrew << " (";
			for (auto x : newPattern)
				std::cout << (int)x << " ";
			std::cout << ")" << std::endl;
#endif
			imageColorIndexes.insert(imageColorIndexes.end(), newPattern.begin(), newPattern.end());
		} // new / old code
		prewCode = code;

		if (nextCodeIndex == maxCode)
		{
			codeSize++;
			maxCode = (1 << (codeSize + 1));
		}
	} // loop
	return imageColorIndexes;
}