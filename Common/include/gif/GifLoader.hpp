#pragma once

#ifndef GIF_LOADER_HPP__
#define GIF_LOADER_HPP__

#include <memory>
#include <string>
#include <map>
#include <vector>

class Image;
class BitStream;
struct RGBPixel;

class GifLoader
{
public:
	GifLoader(const std::string& filename);

	static bool IsGifImage(uint8_t* header, uint32_t size);

	std::unique_ptr<Image> LoadGifImage();

private:
	void LoadColorTable(std::ifstream &ifs, uint8_t flags);
	std::vector<uint8_t> LZWDecode(BitStream& stream, uint8_t lzwMinCodeSize);

	std::string filename;

	bool isGlobalColorTablePresent = false;
	std::map<uint8_t, RGBPixel> colorTable;

};

#endif /* GIF_LOADER_HPP__ */