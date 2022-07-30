#include "PngImage.hpp"
#include "Pixel.hpp"
#include "Exception.hpp"
#include "Common.hpp"

#include <fstream>
#include <vector>
#include <memory>

#include <iostream>
#include <cstdint>
#include <iomanip>

// Chunk header
struct ChunkHdr
{
	uint32_t	lenght;
	std::string	chunkType;
};

// Info Header
PACK(struct IHdr
{
	uint8_t width[4];				// image width (note: in little endian, invert byteorder for value)
	uint8_t height[4];				// image height (note: in little endian, invert byteorder for value)
	uint8_t chanellSize;			// number of bytes per chanel per pixel
	uint8_t colorType;				// 2 = RGB/truecolor, TODO other vals
	uint8_t compressionMethod;		// 0
	uint8_t filterMethod;			// 0
	uint8_t interfaceMethod;		// 0 - no interface 1 - ADAM7
});

// Chunk
struct Chunk
{
	ChunkHdr					header = { 0, { 0,0,0,0 } };
	uint8_t						crc[4] = { 0,0,0,0 }; // Cyclic redundancy check
	std::unique_ptr<uint8_t[]>	data;
};

#define TO_INT(data)  ((data[0] << 8 * 3) | (data[1] << 8 * 2) | (data[2] << 8 * 1) | data[3])

PngImage::PngImage(std::string filename)
{
	LoadData(filename);
}

void PngImage::LoadData(std::string filename)
{
	// Check file
	std::ifstream file(filename, std::ios::binary);
	if (!file.is_open())
	{
		throw RuntimeException("Unable to open file: \"" + filename + "\"");
	}

	// Check if png data
	uint8_t signature[8];
	file.read((char*)signature, sizeof(signature));
	// TODO check if equal to 89 50 4E 47 0D 0A 1A 0A

	std::vector<Chunk> chunks;
	while (file)
	{
		Chunk chunk;

		// Read  size
		uint8_t data[4];
		file.read((char*)data, 4);

		chunk.header.lenght = TO_INT(data);
		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		std::cout << "Size: " << std::dec << chunk.header.lenght << std::endl;		/////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////

		// Read chunk type
		file.read((char*)data, 4);
		chunk.header.chunkType = std::string((char *)data, 4);
		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		std::cout << "Type: " << chunk.header.chunkType << std::endl;;		/////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////

		// Read data
		chunk.data = std::unique_ptr<uint8_t[]>(new uint8_t[chunk.header.lenght]);
		file.read((char*)chunk.data.get(), chunk.header.lenght);
		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		std::cout << "Data: ";		/////////////////////////////////////////////////////////////////////////////
		for (uint32_t i = 0; i < (chunk.header.lenght < 100 ? chunk.header.lenght : 100); i++)		/////////////
		{		/////////////////////////////////////////////////////////////////////////////////////////////////
			std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)chunk.data[i] << " ";		/////
		}		/////////////////////////////////////////////////////////////////////////////////////////////////
		std::cout << std::endl;		/////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////

		// Read crc
		file.read((char*)&chunk.crc, 4);
		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		std::cout << "Crc : ";		/////////////////////////////////////////////////////////////////////////////
		for (int i = 0; i < 4; i++)		/////////////////////////////////////////////////////////////////////////
		{		/////////////////////////////////////////////////////////////////////////////////////////////////
			std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)chunk.crc[i] << " ";		/////////
		}		/////////////////////////////////////////////////////////////////////////////////////////////////
		std::cout << std::endl;		/////////////////////////////////////////////////////////////////////////////
		std::cout << std::endl;		/////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////

		// TODO add to vector
		auto type = chunk.header.chunkType;
		chunks.push_back(std::move(chunk));

		// Last chunk
		if (type == std::string("IEND"))
		{
			break;
		}
	}

	// Process header (ihdr)
	for (auto& chunk : chunks)
	{
		if (chunk.header.chunkType != "IHDR")
		{
			continue;
		}

		// IHdr

		// TODO
		break;
	}

	// TODO
	// TODO
	// TODO

	file.close();
}
