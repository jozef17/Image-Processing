#pragma once

#ifndef PNG_LOADER_HPP__
#define PNG_LOADER_HPP__

#include <string>
#include <vector>
#include <fstream>
#include <memory>

class Image;
class BitStream;
struct Chunk;

class PngLoader
{
public:
	PngLoader(const std::string& filename);

	static bool IsPngImage(uint8_t* header, uint32_t size);

	std::unique_ptr<Image> LoadPngImage();

private:
	std::unique_ptr<Chunk> LoadChnuk(std::ifstream& file);

	void ProcessHeader(std::unique_ptr<Chunk>& header);

	std::unique_ptr<Image> ProcessData(BitStream& bitstream);

	uint8_t Paeth(uint8_t a, uint8_t b, uint8_t c) const noexcept;

	std::string filename;
	uint32_t height = 0;
	uint32_t width = 0;
	uint8_t colorType = 0;

};

#endif /* PNG_LOADER_HPP__ */