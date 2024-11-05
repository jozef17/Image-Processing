#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <iomanip>

#include "Pixel.hpp"

// https://giflib.sourceforge.net/whatsinagif/bits_and_bytes.html
int main(int argc, char* argv[])
{
	// Check file
//	std::ifstream file("D:\\Users\\User\\Pictures\\yellow.gif", std::ios::binary);
	std::ifstream file("D:\\Users\\User\\Pictures\\White.gif", std::ios::binary);
	if (!file.is_open())
	{
		std::cerr << "Cannot open file!" << std::endl;
		return -1;
	}
	
	// Check header
	uint8_t header[6];
	file.read((char*)&header, 6);
	if (header[0] != 0x47 || header[1] != 0x49 || header[2] != 0x46 || header[3] != 0x38
		|| !(header[4] == 0x37 || header[4] == 0x39) || header[5] != 0x61)
	{
		std::cerr << "Not gif!" << std::endl;
		return -2;
	}

	// Logical Screen Descriptor
	uint8_t screenDescriptor[7];
	file.read((char*)&screenDescriptor, 7);

	// Only byte 5 is important
	uint8_t flags = screenDescriptor[4];
	// Is Global Color Table Present - msb
	bool isGlobalColorTablePresent = flags & 1 << 7;
	if (isGlobalColorTablePresent)
	{
		// ??? This or this to use for number of colors
		// TBD 0???0???
		std::cout << "Flags (" << std::setw(2) << std::setfill('0') << std::hex << (int)flags << "): ";
		for (int i = 7; i >= 0; i--)
			std::cout << ((flags & (1 << i)) ? "1" : "0");
		std::cout << std::endl;

		int n = flags >> 4;
		n = n & 0b00000111;

		std::cout << "N    : ";
		for (int i = 7; i >= 0; i--)
			std::cout << ((n & (1 << i)) ? "1" : "0");
		std::cout << std::endl;

		if (n == 0b00000111)
		{
			// Global Color Table
			RGBPixel colors[256];
			for (int i = 0; i < 256; i++)
			{
				file.read((char*)&colors[i], 3);
				std::cout << std::setw(2) << std::setfill('0') << std::hex << i << " " << (int)colors[i].red << " " << (int)colors[i].green << " " << (int)colors[i].blue << std::endl;
			}

			// Graphics Control Extension
			// TODO make optionall & skip
			uint8_t nextBlock[3];
			file.read((char*)&nextBlock, 3);
			if (nextBlock[0] != 0x21 || nextBlock[1] != 0xF9)
			{
				std::cerr << "Missing Graphics Control Extension!" << std::endl;
				return -5;
			}
			
			uint8_t skip;
			for (int i = 0; i < nextBlock[2] + 1; i++)
			{
				file.read((char*)&skip, 1);
			}

			// Image Descriptor
			uint8_t imageDescriptor[10];
			file.read((char*)&imageDescriptor, 10);
			if (imageDescriptor[0] != 0x2C)
			{
				// TODO make somehow dynamic loading the data
				std::cerr << "Expected Image Descriptor!" << std::endl;
				return -6;
			}

			int	width = ((int)imageDescriptor[6] << 8) | imageDescriptor[5];
			int	height = ((int)imageDescriptor[8] << 8) | imageDescriptor[7];

			std::cout << "Width:  " << std::setw(2) << std::setfill('0') << std::hex << (int)imageDescriptor[5] << " " <<
				std::setw(2) << std::setfill('0') << std::hex << (int)imageDescriptor[6] << " = " << std::dec << width << std::endl;
			std::cout << "Height: " << std::setw(2) << std::setfill('0') << std::hex << (int)imageDescriptor[7] << " " <<
				std::setw(2) << std::setfill('0') << std::hex << (int)imageDescriptor[8] << " = " << std::dec << height << std::endl;

			std::cout << "Flags: ";
			for (int i = 7; i >= 0; i--)
				std::cout << ((imageDescriptor[9] & (1 << i)) ? "1" : "0");
			std::cout << std::endl;
			if (imageDescriptor[9] != 0)
			{
				// TODO add handling local color table handling and flags
				std::cerr << "Image Descriptor flags not supported!" << std::endl;
				return -7;
			}

			//
			
			// TODO
		}
		else
		{
			// TODO: add different sizes for color table (tbd msb / lsb bits to number + 1)
			std::cerr << "Unsupported number of colors!" << std::endl;
			return -4;
		}

		// TODO skip Plain Text Extension
		// TODO skip comment Extension
		// TODO skip Application Extension
	}
	else
	{
		// TODO: add support for no global color table (if such image is found)
		std::cerr << "Global color table missing!" << std::endl;
		return -3;
	}

	// TODO ...
	// 08 22 00 f7 1c 48 b0 a0 c1 83 08 13 2a 5c c8 b0 a1 c3 87 10 23 4a 9c 48 b1 a2 c5 8b 18 33 6a dc 68 31 00 3b
	
	do
	{
		uint8_t c = 0;
		file >> c;
		std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)c << " ";
	} while (file.good());

	return 0;
}

// 47 49 46 38 39 61				// Header
// 14 00 14 00 f7 00 00				// Logical Screen Descriptor
// 00 00 00 00 00 33 00 00 66 00 00 99 00 00 cc 00 00 ff 00 2b 00 00 2b 33 00 2b 66 00 2b 99 00 2b cc 00 2b ff 00 55 00 00 55 33 00 55 66 00 55 99 00 55 cc 00 55 ff 00 80 00 00 80 33 00 80 66 00 80 99 00 80 cc 00 80 ff 00 aa 00 00 aa 33 00 aa 66 00 aa 99 00 aa cc 00 aa ff 00 d5 00 00 d5 33 00 d5 66 00 d5 99 00 d5 cc 00 d5 ff 00 ff 00 00 ff 33 00 ff 66 00 ff 99 00 ff cc 00 ff ff 33 00 00 33 00 33 33 00 66 33 00 99 33 00 cc 33 00 ff 33 2b 00 33 2b 33 33 2b 66 33 2b 99 33 2b cc 33 2b ff 33 55 00 33 55 33 33 55 66 33 55 99 33 55 cc 33 55 ff 33 80 00 33 80 33 33 80 66 33 80 99 33 80 cc 33 80 ff 33 aa 00 33 aa 33 33 aa 66 33 aa 99 33 aa cc 33 aa ff 33 d5 00 33 d5 33 33 d5 66 33 d5 99 33 d5 cc 33 d5 ff 33 ff 00 33 ff 33 33 ff 66 33 ff 99 33 ff cc 33 ff ff 66 00 00 66 00 33 66 00 66 66 00 99 66 00 cc 66 00 ff 66 2b 00 66 2b 33 66 2b 66 66 2b 99 66 2b cc 66 2b ff 66 55 00 66 55 33 66 55 66 66 55 99 66 55 cc 66 55 ff 66 80 00 66 80 33 66 80 66 66 80 99 66 80 cc 66 80 ff 66 aa 00 66 aa 33 66 aa 66 66 aa 99 66 aa cc 66 aa ff 66 d5 00 66 d5 33 66 d5 66 66 d5 99 66 d5 cc 66 d5 ff 66 ff 00 66 ff 33 66 ff 66 66 ff 99 66 ff cc 66 ff ff 99 00 00 99 00 33 99 00 66 99 00 99 99 00 cc 99 00 ff 99 2b 00 99 2b 33 99 2b 66 99 2b 99 99 2b cc 99 2b ff 99 55 00 99 55 33 99 55 66 99 55 99 99 55 cc 99 55 ff 99 80 00 99 80 33 99 80 66 99 80 99 99 80 cc 99 80 ff 99 aa 00 99 aa 33 99 aa 66 99 aa 99 99 aa cc 99 aa ff 99 d5 00 99 d5 33 99 d5 66 99 d5 99 99 d5 cc 99 d5 ff 99 ff 00 99 ff 33 99 ff 66 99 ff 99 99 ff cc 99 ff ff cc 00 00 cc 00 33 cc 00 66 cc 00 99 cc 00 cc cc 00 ff cc 2b 00 cc 2b 33 cc 2b 66 cc 2b 99 cc 2b cc cc 2b ff cc 55 00 cc 55 33 cc 55 66 cc 55 99 cc 55 cc cc 55 ff cc 80 00 cc 80 33 cc 80 66 cc 80 99 cc 80 cc cc 80 ff cc aa 00 cc aa 33 cc aa 66 cc aa 99 cc aa cc cc aa ff cc d5 00 cc d5 33 cc d5 66 cc d5 99 cc d5 cc cc d5 ff cc ff 00 cc ff 33 cc ff 66 cc ff 99 cc ff cc cc ff ff ff 00 00 ff 00 33 ff 00 66 ff 00 99 ff 00 cc ff 00 ff ff 2b 00 ff 2b 33 ff 2b 66 ff 2b 99 ff 2b cc ff 2b ff ff 55 00 ff 55 33 ff 55 66 ff 55 99 ff 55 cc ff 55 ff ff 80 00 ff 80 33 ff 80 66 ff 80 99 ff 80 cc ff 80 ff ff aa 00 ff aa 33 ff aa 66 ff aa 99 ff aa cc ff aa ff ff d5 00 ff d5 33 ff d5 66 ff d5 99 ff d5 cc ff d5 ff ff ff 00 ff ff 33 ff ff 66 ff ff 99 ff ff cc ff ff ff 00 00 00 00 00 00 00 00 00 00 00 00 
// 21 f9 04 01 00 00 fc 00			// Graphics Control Extension
// 2c 00 00 00 00 14 00 14 00 00	// Image Descriptor
// 08    22    00 f7 1c 48 b0 a0 c1 83 08 13 2a 5c c8 b0 a1 c3 87 10 23 4a 9c 48 b1 a2 c5 8b 18 33 6a dc 68 31 00 3b