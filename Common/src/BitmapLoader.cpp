#include "BitmapLoader.hpp"
#include "Image.hpp"
#include "Pixel.hpp"
#include "Exception.hpp"
#include "Common.hpp"

#include <fstream>

PACK(struct BITMAPFILEHEADER
{
	unsigned short  bfType;
	unsigned int    bfSize;
	unsigned short  bfReserved1;
	unsigned short  bfReserved2;
	unsigned int    bfOffBits;
});

PACK(struct BITMAPINFOHEADER
{
	unsigned int    biSize;
	int             biWidth;
	int             biHeight;
	unsigned short  biPlanes;
	unsigned short  biBitCount;
	unsigned int    biCompress;
	unsigned int    biSizeImage;
	int             biXPelsPerMeter;
	int             biYPelsPerMeter;
	unsigned int    biClrUsed;
	unsigned int    biClrImportant;
});

BitmapLoader::BitmapLoader(std::string filename) : filename(filename) {}

bool BitmapLoader::IsBitmapImage(uint8_t* header, uint32_t size)
{
	if (size < 2)
	{
		throw RuntimeException("Not enough data to asses the file (bmp)");
	}

	// BM - Windows 3.1x, 95, NT, ...
	// BA - OS / 2 y
	if (header[0] == 'B' && (header[1] == 'M' || header[1] == 'A'))
	{
		return true;
	}
	// TBD: other os versions

	return false;
}

std::unique_ptr<Image> BitmapLoader::LoadBitmapImage()
{
	// Check file
	std::ifstream file(filename, std::ios::binary);
	if (!file.is_open())
	{
		throw RuntimeException("Unable to open file: \"" + filename + "\"");
	}

	// Read header
	BITMAPFILEHEADER bmpHeader;
	BITMAPINFOHEADER bmpInfo; // Info

	file.read((char*)&bmpHeader, sizeof(bmpHeader));
	file.read((char*)&bmpInfo, sizeof(bmpInfo));

	if (bmpHeader.bfType != 0x4D42)
	{
		file.close();
		throw RuntimeException("Not a bmp file (" + filename + ")!");
	}

	std::unique_ptr<Image> img = std::make_unique<Image>(bmpInfo.biWidth, bmpInfo.biHeight, Image::StartPosition::BottomLeft);
	unsigned long offset = bmpHeader.bfOffBits;
	unsigned long size = bmpInfo.biSizeImage;

	// Load data
	file.seekg(offset); // go to start of data

	for (uint32_t y = 0; y < bmpInfo.biHeight; y++)
	{
		for (uint32_t x = 0; x < bmpInfo.biWidth; x++)
		{
			// read pixel
			uint8_t data[3];
			file.read((char*)&data, 3);
			// bmp stores color data in inverted order
			img->SetPixel(x, y, Pixel(RGBPixel{ data[2], data[1], data[0] }));
		}
	}

	file.close();
	return img;
}
