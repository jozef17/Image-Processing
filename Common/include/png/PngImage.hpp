#pragma once

#ifndef PNG_IMAGE_HPP__
#define PNG_IMAGE_HPP__

#include <string>
#include <vector>
#include <fstream>

#include "Image.hpp"

class BitStream;

// Chunk header
struct ChunkHdr
{
	uint32_t	lenght;
	std::string	chunkType;
};

// Chunk
struct Chunk
{
	ChunkHdr					header = { 0, { 0,0,0,0 } };
	uint8_t						crc[4] = { 0,0,0,0 }; // Cyclic redundancy check
	std::unique_ptr<uint8_t[]>	data;
};

class PngImage : public Image
{
public:
	PngImage(const std::string& filename);

private:
	std::unique_ptr<Chunk> LoadChnuk(std::ifstream& file);

	void ProcessHeader(std::unique_ptr<Chunk> &header);

	void ProcessData(BitStream& bitstream);

};

#endif /* PNG_IMAGE_HPP__ */