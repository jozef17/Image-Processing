#include "BitmapLoader.hpp"
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

BitmapImage::BitmapImage(std::string filename)
{
	this->startPosition = Image::StartPosition::TopLeft;

	// Check file
	std::ifstream file(filename, std::ios::binary);
	if (!file.is_open())
	{
		throw RuntimeException("Unable to open file: \"" + filename + "\"");
	}

	// Read header
	auto bmpHeader = (BITMAPFILEHEADER*)new unsigned char[sizeof(BITMAPFILEHEADER)];
	auto bmpInfo   = (BITMAPINFOHEADER*)new unsigned char[sizeof(BITMAPINFOHEADER)]; // Info

	file.read((char*)bmpHeader, sizeof(BITMAPFILEHEADER));
	file.read((char*)bmpInfo, sizeof(BITMAPINFOHEADER));

	if (bmpHeader->bfType != 0x4D42)
	{
		delete bmpHeader;
		delete bmpInfo;
		file.close();

		throw RuntimeException("Not a bmp file (" + filename + ")!");
	}

	this->height = bmpInfo->biHeight;
	this->width = bmpInfo->biWidth;

	unsigned long offset = bmpHeader->bfOffBits;
	unsigned long size = bmpInfo->biSizeImage;

	delete bmpHeader;
	delete bmpInfo;

	// Load data
	file.seekg(offset); // go to start of data

	this->image = std::unique_ptr<std::unique_ptr<Pixel>[]>(new std::unique_ptr<Pixel>[width * height]);
	for (uint32_t i = 0; i < this->width * this->height; i++)
	{
		// read pixel
		uint8_t data[3];
		file.read((char*)&data, 3);
		// bmp stores color data in inverted order
		RGBPixel rgb{ data[2], data[1], data[0]};
		this->image[i] = std::unique_ptr<Pixel>(new Pixel(rgb));
	}

	file.close();
}
