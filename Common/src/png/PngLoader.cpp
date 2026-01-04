#include "png/PngLoader.hpp"
#include "png/PngValueReader.hpp"
#include "png/Inflate.hpp"

#include "BitStream.hpp"
#include "Pixel.hpp"
#include "Image.hpp"
#include "Exception.hpp"
#include "Common.hpp"

#include <memory>
#include <cstdint>
#include <algorithm>

#ifdef ENABLE_LOGS
#include <iostream>
#include <iomanip>
#endif

#define TO_INT(data)  ((data[0] << 8 * 3) | (data[1] << 8 * 2) | (data[2] << 8 * 1) | data[3])

// Chunk header
struct ChunkHdr
{
	uint32_t	length;
	std::string	chunkType;
};

// Chunk
struct Chunk
{
	ChunkHdr					header = { 0, { 0,0,0,0 } };
	uint8_t						crc[4] = { 0,0,0,0 }; // Cyclic redundancy check
	std::unique_ptr<uint8_t[]>	data;
};

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

PngLoader::PngLoader(const std::string& filename) : filename(filename) {}

bool PngLoader::IsPngImage(uint8_t* header, uint32_t size)
{
	if (size < 8)
	{
		throw RuntimeException("Not enough data to asses the file (png)");
	}

	uint8_t reference[8] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
	return std::memcmp(header, reference, 8) == 0;
}

std::unique_ptr<Image> PngLoader::LoadPngImage()
{
	// Check file
	std::ifstream file(this->filename, std::ios::binary);
	if (!file.is_open())
	{
		throw RuntimeException("Unable to open file: \"" + filename + "\"");
	}

	// Check if png data
	uint8_t signature[8] = { 0 };
	file.read((char*)signature, sizeof(signature));
	if (!IsPngImage(signature,sizeof(signature)))
	{
		throw RuntimeException("Not a png file (" + filename + ")!");
	}

	// Load png chunks
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
			bitstream.Append(std::move(chunk->data), chunk->header.length);
		}
		else if(chunk->header.chunkType == "PLTE")
		{
			ProcessColorPalette(chunk);
		}
		else if (chunk->header.chunkType == "IEND")
		{
			break;
		}
	}
	file.close();

	return ProcessData(bitstream);
}

std::unique_ptr<Chunk> PngLoader::LoadChnuk(std::ifstream& file)
{
	std::unique_ptr<Chunk> chunk = std::make_unique<Chunk>();

	// Read size
	uint8_t data[4] = { 0 };
	file.read((char*)data, 4);
	chunk->header.length = TO_INT(data);

	// Read chunk type
	file.read((char*)data, 4);
	chunk->header.chunkType = std::string((char*)data, 4);

	// Read data
	chunk->data = std::unique_ptr<uint8_t[]>(new uint8_t[chunk->header.length]);
	file.read((char*)chunk->data.get(), chunk->header.length);

	// Read crc
	file.read((char*)&chunk->crc, 4);

#ifdef ENABLE_LOGS
	std::cout << "[PngImage::LoadChnuk] - Size: " << std::dec << chunk->header.length << std::endl;
	std::cout << "                      - Type: " << chunk->header.chunkType << std::endl;
	std::cout << "                      - Data: ";
	for (uint32_t i = 0; i < (chunk->header.length < 100 ? chunk->header.length : 100); i++)
	{
		for (uint32_t j = 0; j < 8; j++)
		{
			auto bit = chunk->data[i] & 1 << j;
			std::cout << (bit > 0 ? "1" : "0");
		}
		std::cout << " ";
	}

	if (chunk->header.length > 100)
	{
		std::cout << "...";
	}
	std::cout << std::endl;
#endif
	return std::move(chunk);
}

void PngLoader::ProcessHeader(std::unique_ptr<Chunk> &ihdrChunk)
{
	IHDR ihdr = { 0 };
	std::memcpy(&ihdr, ihdrChunk->data.get(), sizeof(IHDR));

#ifdef ENABLE_LOGS
	std::cout << "[PngImage::ProcessHeader] " << ihdrChunk->header.chunkType << std::endl;
	std::cout << "                          - width:             " << << std::dec << (int)(TO_INT(ihdr.width)) << std::endl;
	std::cout << "                          - height:            " << std::dec << (int)(TO_INT(ihdr.height)) << std::endl;
	std::cout << "                          - chanellSize:       "
		<< std::setw(2) << std::setfill('0') << std::hex << (int)ihdr.chanellSize << std::endl;
	std::cout << "                          - colorType:         "
		<< std::setw(2) << std::setfill('0') << std::hex << (int)ihdr.colorType << std::endl;
#endif

	// NOTE: ADAM7 not supported
	if (ihdr.interfaceMethod != 0)
	{
		throw RuntimeException("Unsupported interface method: \"" + std::to_string(ihdr.interfaceMethod) + "\"");
	}

	// Only "LZ77" is supported by png
	if (ihdr.compressionMethod != 0)
	{
		throw RuntimeException("Unsupported compression method: \"" + std::to_string(ihdr.compressionMethod) + "\"");
	}

	// Only filter method 0 is supported by png
	if (ihdr.filterMethod != 0)
	{
		throw RuntimeException("Unsupported filter method: \"" + std::to_string(ihdr.filterMethod) + "\"");
	}

	if(ihdr.chanellSize == 16)
	{
		throw RuntimeException("16bit colors are not supported!");
	}

	// Allocate image data
	this->width       = (TO_INT(ihdr.width));
	this->height      = (TO_INT(ihdr.height));
	this->colorType   = static_cast<ColorType>(ihdr.colorType);
	this->chanellSize = ihdr.chanellSize;

	CheckChanellSize();
}

void PngLoader::CheckChanellSize() const
{
	std::vector<uint8_t> allowedChannelSizes = { 0,16 };
	if (this->colorType == ColorType::Indexed)
	{
		allowedChannelSizes = { 1,2,4,8 };
	}
	else if (this->colorType == ColorType::GrayScale)
	{
		allowedChannelSizes = { 1,2,4,8,16 };
	}

	if (std::none_of(allowedChannelSizes.begin(), 
					 allowedChannelSizes.end(), 
					 [this](uint8_t val) { return this->chanellSize != val;}))
	{
		throw RuntimeException("Invalid cpombination of color type (" + std::to_string((int)this->colorType) + ") and channel size (" + std::to_string((int)this->chanellSize) + ")!");
	}
}

void PngLoader::ProcessColorPalette(std::unique_ptr<Chunk>& palette)
{
	if (palette->header.length % 3 != 0)
	{
		throw RuntimeException("Invalid Palette size!");
	}

	for (int i = 0; i < palette->header.length; i += 3)
	{
		RGBAPixel pixel = { palette->data[i], palette->data[i + 1], palette->data[i + 2], (uint8_t)255 };
		this->colorPalette[i / 3] = pixel;
	}
}

std::unique_ptr<Image> PngLoader::ProcessData(BitStream& bitstream)
{
	Inflate decoder(bitstream);
	auto decodedBytes = decoder.Decode();

	std::unique_ptr<Image> image = std::make_unique<Image>(this->width, this->height);

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
	std::cout << "[PngImage::ProcessData] Pixels:" << std::endl;
#endif

	// "Defilter" data
	PngValueReader reader(std::move(decodedBytes), this->chanellSize);
	for (uint32_t y = 0; y < this->height; y++) // y
	{
		auto filterType = reader.GetFilterMethod();
		for (uint32_t x = 0; x < this->width; x++) // x
		{
			uint16_t red   = 0;
			uint16_t green = 0;
			uint16_t blue  = 0;
			uint16_t alpha = 255;

			// RGB & RGBA
			if (this->colorType == ColorType::TrueColorAlpha ||
				this->colorType == ColorType::TrueColor)
			{
				red   = reader.GetNextValue();
				green = reader.GetNextValue();
				blue  = reader.GetNextValue();
			}
			// Gray Scale
			else if (this->colorType == ColorType::GrayScaleAlpha ||
       				 this->colorType == ColorType::GrayScale)
			{
				red   = reader.GetNextValue();
				green = red;
				blue  = red;
			}

			// Alpha channel
			if (this->colorType == ColorType::TrueColorAlpha || 
				this->colorType == ColorType::GrayScaleAlpha)
			{
				alpha = reader.GetNextValue();
			}

			// Prepare pixel
			RGBAPixel pixel = { red, green, blue, alpha };
			if (this->colorType == ColorType::Indexed)
			{
				pixel = this->colorPalette[reader.GetNextValue()];
			}

			// Defileter 
			Defilter(filterType, image, x, y, pixel);
			image->SetPixel(x, y, Pixel{ pixel });
		}
#ifdef ENABLE_LOGS
		std::cout << std::endl;
#endif
	}
	return image;
}

void PngLoader::Defilter(uint8_t filterType, std::unique_ptr<Image>& image, uint32_t x, uint32_t y, RGBAPixel &pixel)
{
	RGBAPixel leftPixel     = { 0,0,0,0 }; // a
	RGBAPixel abovePixel    = { 0,0,0,0 }; // b
	RGBAPixel diagonalPixel = { 0,0,0,0 }; // c

	// Get Neighbouring Pixels
	if (x > 0)
	{
		leftPixel = image->GetPixel(x - 1, y).ToRGBA();
		if (y > 0)
		{
			diagonalPixel = image->GetPixel(x - 1, y - 1).ToRGBA();
		}
	}
	if (y > 0)
	{
		abovePixel = image->GetPixel(x, y - 1).ToRGBA();
	}

	// Apply filter
	switch (filterType)
	{
	case 0: // None
		break;
	case 1:
		pixel.red   += leftPixel.red;
		pixel.green += leftPixel.green;
		pixel.blue  += leftPixel.blue;
		pixel.alpha += leftPixel.alpha;
		break;
	case 2: // Up
		pixel.red   += abovePixel.red;
		pixel.green += abovePixel.green;
		pixel.blue  += abovePixel.blue;
		pixel.alpha += abovePixel.alpha;
		break;
	case 3: // Average
		pixel.red   += static_cast<uint8_t>(std::floor((abovePixel.red + leftPixel.red) / 2));
		pixel.green += static_cast<uint8_t>(std::floor((abovePixel.green + leftPixel.green) / 2));
		pixel.blue  += static_cast<uint8_t>(std::floor((abovePixel.blue + leftPixel.blue) / 2));
		pixel.alpha += static_cast<uint8_t>(std::floor((abovePixel.alpha + leftPixel.alpha) / 2));
		break;
	case 4: // Peath
		pixel.red   += Paeth(leftPixel.red, abovePixel.red, diagonalPixel.red);
		pixel.green += Paeth(leftPixel.green, abovePixel.green, diagonalPixel.green);
		pixel.blue  += Paeth(leftPixel.blue, abovePixel.blue, diagonalPixel.blue);
		pixel.alpha += Paeth(leftPixel.alpha, abovePixel.alpha, diagonalPixel.alpha);
		break;
	default:
		throw RuntimeException("Invalid filtering method: " + std::to_string(filterType));
	}
#ifdef ENABLE_LOGS
	std::cout << std::setw(3) << std::setfill('0') << std::dec << (int)red << " "
		<< std::setw(3) << std::setfill('0') << std::dec << (int)green << " "
		<< std::setw(3) << std::setfill('0') << std::dec << (int)blue << " "
		<< std::setw(3) << std::setfill('0') << std::dec << (int)alpha << ", ";
#endif
}

uint8_t PngLoader::Paeth(uint8_t a, uint8_t b, uint8_t c) const noexcept
{
	auto p  = a + b - c;
	auto pa = std::abs(p - a);
	auto pb = std::abs(p - b);
	auto pc = std::abs(p - c);
	
	if (pa <= pb && pa <= pc)
	{
		return a;
	}
	else if (pb <= pc)
	{
		return b;
	}
	return c;
}
