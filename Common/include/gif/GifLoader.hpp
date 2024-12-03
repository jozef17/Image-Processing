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
	std::unique_ptr<Image> ProcessImageData(std::vector<uint8_t> imageColorIndexes);

	std::string filename;
	uint32_t width;
	uint32_t height;

	bool isGlobalColorTablePresent = false;
	std::map<uint8_t, RGBPixel> colorTable;

};

#endif /* GIF_LOADER_HPP__ */