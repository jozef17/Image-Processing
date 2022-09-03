#include "png/PngImage.hpp"
#include "png/Inflate.hpp"
#include "png/BitStream.hpp"

#include "Pixel.hpp"
#include "Exception.hpp"
#include "Common.hpp"

#include <memory>
#include <cstdint>

#ifdef ENABLE_LOGS
#include <iostream>
#include <iomanip>
#endif

#define TO_INT(data)  ((data[0] << 8 * 3) | (data[1] << 8 * 2) | (data[2] << 8 * 1) | data[3])

// Info Header
PACK(struct IHDR
{
	uint8_t width[4];				// image width
	uint8_t height[4];				// image height
	uint8_t chanellSize;			// number of bytes per chanel per pixel
	uint8_t colorType;				// 2 = RGB/truecolor, TODO other vals
	uint8_t compressionMethod;		// 0
	uint8_t filterMethod;			// 0
	uint8_t interfaceMethod;		// 0 - no interface 1 - ADAM7
});

enum class ColorType : uint8_t
{
	GrayScale		= 0b000,
	TrueColor		= 0b010, // RGB
	Indexed			= 0b011,
	GrayScaleAlpha	= 0b100,
	TrueColorAlpha	= 0b110 // RGBA
};

PngImage::PngImage(const std::string& filename)
{
	// Check file
	std::ifstream file(filename, std::ios::binary);
	if (!file.is_open())
	{
		throw RuntimeException("Unable to open file: \"" + filename + "\"");
	}

	// Check if png data
	uint8_t signature[8] = { 0 };
	file.read((char*)signature, sizeof(signature));
	uint8_t reference[8] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
	if (std::memcmp(signature, reference, 8) != 0)
	{
		throw RuntimeException("Not a png file (" + filename + ")!");
	}

	BitStream bitstream;
	while (file)
	{
		auto chunk = LoadChnuk(file);

		if (chunk->header.chunkType == "IHDR")
		{
			ProcessHeader(chunk);
		}
		else if (chunk->header.chunkType == "IDAT")
		{
			bitstream.Append(std::move(chunk->data), chunk->header.lenght);
		}

		// Handle other chunks if needed

		else if (chunk->header.chunkType == "IEND")
		{
			break;
		}
	}
	file.close();

	ProcessData(bitstream);
}

std::unique_ptr<Chunk> PngImage::LoadChnuk(std::ifstream& file)
{
	std::unique_ptr<Chunk> chunk = std::make_unique<Chunk>();

	// Read size
	uint8_t data[4] = { 0 };
	file.read((char*)data, 4);
	chunk->header.lenght = TO_INT(data);
#ifdef ENABLE_LOGS
	std::cout << "[PngImage::LoadChnuk] - Size: " << std::dec << chunk->header.lenght << std::endl;
#endif

	// Read chunk type
	file.read((char*)data, 4);
	chunk->header.chunkType = std::string((char*)data, 4);
#ifdef ENABLE_LOGS
	std::cout << "                      - Type: " << chunk->header.chunkType << std::endl;
#endif

	// Read data
	chunk->data = std::unique_ptr<uint8_t[]>(new uint8_t[chunk->header.lenght]);
	file.read((char*)chunk->data.get(), chunk->header.lenght);
#ifdef ENABLE_LOGS
	std::cout << "                      - Data: ";
	for (uint32_t i = 0; i < (chunk->header.lenght < 100 ? chunk->header.lenght : 100); i++)
	{
		for (uint32_t j = 0; j < 8; j++)
		{
			auto bit = chunk->data[i] & 1 << j;
			std::cout << (bit > 0 ? "1" : "0");
		}
		std::cout << " ";
	}

	if (chunk->header.lenght > 100)
	{
		std::cout << "...";
	}
	std::cout << std::endl;
#endif

	// Read crc
	file.read((char*)&chunk->crc, 4);
#ifdef ENABLE_LOGS
	std::cout << "                      - Crc: ";
	for (int i = 0; i < 4; i++)
	{
		std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)chunk->crc[i] << " ";
	}
	std::cout << std::endl;
#endif
	return std::move(chunk);
}

void PngImage::ProcessHeader(std::unique_ptr<Chunk> &ihdrChunk)
{
	IHDR ihdr = { 0 };
	std::memcpy(&ihdr, ihdrChunk->data.get(), sizeof(IHDR));

#ifdef ENABLE_LOGS
	std::cout << "[PngImage::ProcessHeader] " << ihdrChunk->header.chunkType << std::endl;

	std::cout << "                          - width:  ";
	for (int i = 0; i < 4; i++)
	{
		std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)ihdr.width[i] << " ";
	}
	std::cout << " (" << std::dec << (int)(TO_INT(ihdr.width)) << ") ";
	std::cout << " [" << std::dec << (int)(this->width) << "]" << std::endl;

	std::cout << "                          - height: ";
	for (int i = 0; i < 4; i++)
	{
		std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)ihdr.height[i] << " ";
	}
	std::cout << " (" << std::dec << (int)(TO_INT(ihdr.height)) << ") ";
	std::cout << " [" << std::dec << (int)(this->height) << "]" << std::endl;

	std::cout << "                          - chanellSize:       "
		<< std::setw(2) << std::setfill('0') << std::hex << (int)ihdr.chanellSize << std::endl;
	std::cout << "                          - colorType:         "
		<< std::setw(2) << std::setfill('0') << std::hex << (int)ihdr.colorType << std::endl;
	std::cout << "                          - compressionMethod: "
		<< std::setw(2) << std::setfill('0') << std::hex << (int)ihdr.compressionMethod << std::endl;
#endif

	// Check unsupported types
	// NOTE: only RGB & RGBA supported
	// TODO: implement other color types when needed
	if (static_cast<ColorType>(ihdr.colorType) != ColorType::TrueColor &&
		static_cast<ColorType>(ihdr.colorType) != ColorType::TrueColorAlpha)
	{
		throw RuntimeException("Unsupported color type: \"" + std::to_string(ihdr.colorType) + "\"");
	}

	// NOTE: ADAM7 not supported
	if (ihdr.interfaceMethod != 0)
	{
		throw RuntimeException("Unsupported interface method: \"" + std::to_string(ihdr.interfaceMethod) + "\"");
	}

	// Only "LZ77" is supported by png
	if (ihdr.compressionMethod != 0)
	{
		throw RuntimeException("Unsupported compression method: \"" + std::to_string(ihdr.interfaceMethod) + "\"");
	}

	// Only filter method 0 is supported by png
	if (ihdr.filterMethod != 0)
	{
		throw RuntimeException("Unsupported filter method: \"" + std::to_string(ihdr.interfaceMethod) + "\"");
	}

	if (ihdr.chanellSize != 8)
	{
		throw RuntimeException("Unsupported chanell size: \"" + std::to_string(ihdr.chanellSize) + "\"");
	}

	// Allocate image data
	this->width = (TO_INT(ihdr.width));
	this->height = (TO_INT(ihdr.height));
	this->colorType = ihdr.colorType;
}

void PngImage::ProcessData(BitStream& bitstream)
{
	Inflate decoder(bitstream);
	auto decodedBytes  = decoder.Decode();
	auto bytesPerPixel = static_cast<ColorType>(this->colorType) == ColorType::TrueColor ? 3 : 4;

#ifdef ENABLE_LOGS
	std::cout << "[PngImage::ProcessData] Decoded Data:" << std::endl;
	for (int a = 0; a < decodedBytes.size(); a++)
	{
		std::cout << std::setw(3) << std::setfill('0') << std::dec << (int)decodedBytes.at(a) << " ";
		if (a % (bytesPerPixel * width + 1) == (bytesPerPixel * width))
		{
			std::cout << std::endl;
		}
	}
	std::cout << std::endl;
#endif

	this->image = std::unique_ptr<std::unique_ptr<Pixel>[]>(new std::unique_ptr<Pixel>[this->width * this->height]);

	auto start = 0;
	while (decodedBytes.size() < this->width * this->height * bytesPerPixel + this->height)
	{
		decodedBytes.push_back(decodedBytes.at(start++));
	}
#ifdef ENABLE_LOGS
	std::cout << "[PngImage::ProcessData] Pixels:" << std::endl;
#endif

	// "Defilter" data
	auto loc = 0;
	for (uint32_t i = 0; i < this->height; i++) // y
	{
		auto filterType = decodedBytes.at(loc++); // get filter method for current row
		for (uint32_t j = 0; j < this->width; j++) // x
		{
			auto red   = decodedBytes.at(loc++);
			auto green = decodedBytes.at(loc++);
			auto blue  = decodedBytes.at(loc++);
			uint8_t alpha = 255;
			if (this->colorType == (uint8_t)ColorType::TrueColorAlpha)
			{
				alpha = decodedBytes.at(loc++);
			}

			switch (filterType)
			{
			case 0:
			{
				// None
				break;
			}
			case 1:
			{
				RGBAPixel leftPixel = { 0,0,0,0 };
				if (j > 0)
				{
					leftPixel = GetPixel(j - 1, i).ToRGBA();
				}

				red   += leftPixel.red;
				green += leftPixel.green;
				blue  += leftPixel.blue;
				alpha += leftPixel.alpha;

				break;
			}
			case 2:
			{
				// Up
				RGBAPixel abovePixel = { 0,0,0,0 }; // b
				if (i > 0)
				{
					abovePixel = GetPixel(j, i - 1).ToRGBA();
				}

				red   += abovePixel.red;
				green += abovePixel.green;
				blue  += abovePixel.blue;
				alpha += abovePixel.alpha;

				break;
			}
			case 3:
			{
				// Average
				RGBAPixel leftPixel  = { 0,0,0,0 }; // a
				RGBAPixel abovePixel = { 0,0,0,0 }; // b
				if (j > 0)
				{
					leftPixel = GetPixel(j - 1, i).ToRGBA();
				}

				if (i > 0)
				{
					abovePixel = GetPixel(j, i - 1).ToRGBA();
				}

				red   += static_cast<uint8_t>(std::floor((abovePixel.red   + leftPixel.red)   / 2));
				green += static_cast<uint8_t>(std::floor((abovePixel.green + leftPixel.green) / 2));
				blue  += static_cast<uint8_t>(std::floor((abovePixel.blue  + leftPixel.blue)  / 2));
				alpha += static_cast<uint8_t>(std::floor((abovePixel.alpha + leftPixel.alpha) / 2));

				break;
			}
			case 4:
			{
				// Peath
				RGBAPixel leftPixel      = { 0,0,0,0 }; // a
				RGBAPixel abovePixel     = { 0,0,0,0 }; // b
				RGBAPixel aboveLeftPixel = { 0,0,0,0 }; // c
				if (j > 0)
				{
					leftPixel = GetPixel(j - 1, i).ToRGBA();
					if (i > 0)
					{
						aboveLeftPixel = GetPixel(j - 1, i - 1).ToRGBA();
					}
				}
				if (i > 0)
				{
					abovePixel = GetPixel(j, i - 1).ToRGBA();
				}
				
				red   += Paeth(leftPixel.alpha, abovePixel.red, aboveLeftPixel.alpha);
				green += Paeth(red, abovePixel.green, abovePixel.red);
				blue  += Paeth(green, abovePixel.blue, abovePixel.red);
				alpha += Paeth(blue, abovePixel.alpha, abovePixel.blue);

				break;
			}
			default:
				throw RuntimeException("Invalid filtering method: " + std::to_string(filterType));
			}
#ifdef ENABLE_LOGS
			std::cout << std::setw(3) << std::setfill('0') << std::dec << (int)red << " "
				<< std::setw(3) << std::setfill('0') << std::dec << (int)green << " "
				<< std::setw(3) << std::setfill('0') << std::dec << (int)blue << " "
				<< std::setw(3) << std::setfill('0') << std::dec << (int)alpha << " "
				<< ", ";
#endif
			RGBAPixel rgba{ red, green, blue, alpha };
			this->image[i * this->width + j] = std::unique_ptr<Pixel>(new Pixel(rgba));
		}
#ifdef ENABLE_LOGS
		std::cout << std::endl;
#endif
	}
}

uint8_t PngImage::Paeth(uint8_t a, uint8_t b, uint8_t c)
{
	auto p  = a + b - c;
	auto pa = std::abs(p - a);
	auto pb = std::abs(p - b);
	auto pc = std::abs(p - c);
	if(pa <= pb && pa <= pc) return a;
	else if(pb <= pc) return b;
	else return c;
}