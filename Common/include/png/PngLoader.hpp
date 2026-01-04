#pragma once

#ifndef PNG_LOADER_HPP__
#define PNG_LOADER_HPP__

#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include <map>

class Image;
class BitStream;

struct Chunk;
struct RGBAPixel;

class PngLoader
{
public:
	PngLoader(const std::string& filename);

	/// <summary>
	/// Checks if the header bits match png signature
	/// </summary>
	/// <param name="header">Header bits</param>
	/// <param name="size">Header size</param>
	/// <returns>True if header matches png, false otherwise. 
	///	If not enough data is provided, an exception is thrown</returns>
	static bool IsPngImage(uint8_t* header, uint32_t size);

	std::unique_ptr<Image> LoadPngImage();

private:
	std::unique_ptr<Chunk> LoadChnuk(std::ifstream& file);

	void ProcessHeader(std::unique_ptr<Chunk>& header);

	/// <summary>
	///	Check compatibility of colorType and chanell size based on png spec
	/// </summary>
	void CheckChanellSize() const;

	void ProcessColorPalette(std::unique_ptr<Chunk>& palette);

	std::unique_ptr<Image> ProcessData(BitStream& bitstream);

	void Defilter(uint8_t filterType, std::unique_ptr<Image>& image, uint32_t x, uint32_t y, RGBAPixel &pixel);

	uint8_t Paeth(uint8_t a, uint8_t b, uint8_t c) const noexcept;

	enum class ColorType : uint8_t
	{
		GrayScale      = 0b000,
		TrueColor      = 0b010, // RGB
		Indexed        = 0b011,
		GrayScaleAlpha = 0b100,
		TrueColorAlpha = 0b110, // RGBA
		Invalid        = 0xff
	};

	std::string filename;
	
	uint32_t height = 0;
	uint32_t width  = 0;

	uint8_t chanellSize = 0;
	ColorType colorType = ColorType::Invalid;

	/// <summary>
	/// Color palette for indexed colors
	/// </summary>
	std::map<uint8_t, RGBAPixel> colorPalette;
};

#endif /* PNG_LOADER_HPP__ */