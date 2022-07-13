#include "BitmapImage.hpp"
#include "Pixel.hpp"
#include "Exception.hpp"

#include <fstream>

#ifdef __GNUC__
#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#endif

#ifdef _MSC_VER
#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop))
#endif

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
	for (auto i = 0; i < this->width * this->height; i++)
	{
		RGBPixel rgb;
		file.read((char*)&rgb, 3);
		this->image[i] = std::unique_ptr<Pixel>(new Pixel(rgb));
	}

	file.close();
}
