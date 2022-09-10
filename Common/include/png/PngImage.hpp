#pragma once

#ifndef PNG_IMAGE_HPP__
#define PNG_IMAGE_HPP__

#include <string>
#include <vector>
#include <fstream>

#include "Image.hpp"

class BitStream;
struct Chunk;

class PngImage : public Image
{
public:
	PngImage(const std::string& filename);

private:
	std::unique_ptr<Chunk> LoadChnuk(std::ifstream& file);

	void ProcessHeader(std::unique_ptr<Chunk> &header);

	void ProcessData(BitStream& bitstream);

	uint8_t Paeth(uint8_t a, uint8_t b, uint8_t c) const noexcept;

	uint8_t colorType;

};

#endif /* PNG_IMAGE_HPP__ */