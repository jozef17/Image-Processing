#include "jpg/JpegImage.hpp"

#include "Exception.hpp"
#include <fstream>
#include <sstream>
#include <cstdint>
#include <iomanip>

// TMP ////////////////////////////////////////////////////
#include <iostream>
#include <vector>
// TMP ////////////////////////////////////////////////////

struct Segment
{
	uint8_t marker[2];
	uint8_t size[2];
	std::vector<uint8_t> data;
};

void LogSegment(const Segment& segment)
{
	auto size = ((int)segment.size[0] << 8 | (int)segment.size[1]) - 2;
	std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)segment.marker[0] << " " << (int)segment.marker[1] << ", " <<
		std::dec << size << "(" << segment.data.size() << "): ";
	for (auto c : segment.data)
	{
		std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)c << " ";
	}
	std::cout << std::endl;
}

JpegImage::JpegImage(const std::string& filename)
{
	LoadImage(filename);
}

void JpegImage::LoadImage(const std::string& filename)
{
	std::ifstream file(filename, std::ios::binary);

	uint8_t buffer[128];
	file.read((char*)&buffer, 2); // Skip 
	if (buffer[0] != 0xff && buffer[1] != 0xd8)
	{
		throw RuntimeException("Not a jpeg image!");
	}

	Segment segment;
	while (file.good())
	{
		file.read((char*)&buffer, sizeof(buffer));

		for (int i = 0; i < sizeof(buffer); i++)
		{
			// New Segment marker
			if (buffer[i] == 0xff)
			{
				// Process non-empty segment
				if (!segment.data.empty())
				{
					ProcessSegment(segment);
				}

				// Check for end-of-file segment
				if (buffer[i + 1] == 0xd9)
				{
					break;
				}

				segment.data.clear();
				segment.marker[0] = buffer[i];
				segment.marker[1] = buffer[i + 1];
				segment.size[0]   = buffer[i + 2];
				segment.size[1]   = buffer[i + 3];

				i += 4;
			}
			segment.data.push_back(buffer[i]);
		}
	}

	/*
	for (auto& s : segments)
	{
		int val = s.size[0] << 8 | s.size[1];
		std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)s.marker[0] << " " << (int)s.marker[1] << ": ("
			<< std::dec << val << " - " << std::setw(2) << std::setfill('0') << std::hex << (int)s.size[0] << " "
			<< (int)s.size[1] << " | " << std::dec << (s.data.size() + 2) << ") ";

		for (auto byte : s.data)
		{
			std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)byte << " ";
		}
		std::cout << std::endl;
	}
	*/

	// TODO
}

void JpegImage::ProcessSegment(const Segment& segment)
{
	LogSegment(segment);

	// Handle APPn segment
	/*if (segment.marker[1] >= 0xe0 && segment.marker[1] <= 0xef)
	{
		ProcessAppn(segment);
		return;
	}*/

	switch (segment.marker[1])
	{
	case 0xe0:
		ProcessAppn(segment);
		break;
	case 0xdb:
		// Process quantization tables
		ProcessDqt(segment);
		break;
	case 0xc0:
	case 0xc2:
		// Image metadata "Start of frame"
		ProcessSf(segment);
		break;
	case 0xda:
		// Start of scan
		ProcessSos(segment);
		break;
	case 0xc4:
		// Huffman table
		ProcessDht(segment);
	default:
		// Ignore unknown segments
		break;
//		throw RuntimeException("Unsupported segment " + std::to_string((int)segment.marker[1]) + "!");
	}
	//
}

void JpegImage::ProcessAppn(const Segment& segment)
{
//	// Ignore APP1 marker
//	if (segment.marker[1] == 0xe1)
//	{
//		return;
//	}

//	// Only APP0 is supported
//	if (segment.marker[1] != 0xe0)
//	{
//		throw RuntimeException("Unsupported app marker!");
//	}

//	// Check length
//	if (segment.size[0] != 0x00 || segment.size[1] != 0x10)
//	{
//		throw RuntimeException("Invalid APPn segment size!");
//	}

	// JFIF Identifier & JFIF version check
	constexpr uint8_t checkSequence[7] = { 0x4a, 0x46, 0x49, 0x46, 0x00, 0x01, 0x01 };
	for (uint8_t c = 0; c < 7; c++)
	{
		if (segment.data[c] != checkSequence[c])
		{
			std::stringstream errMsg;
			errMsg << "Invalid Application data: " << std::setw(2) << std::setfill('0') << std::hex <<
				segment.data[0] << " " << segment.data[1] << " " << segment.data[2] << " " <<
				segment.data[3] << " " << segment.data[4] << " " << segment.data[5] << " " <<
				segment.data[6];

			throw RuntimeException(errMsg.str());
		}
	}
}

void JpegImage::ProcessDqt(const  Segment& segment)
{
	// Supports only 8bit mode - 1st half of 1st byte determines 8 (0) or 16 (!0) bit mode 
	if (segment.data[0] > 4)
	{
		throw RuntimeException("Unsupported quantization table type!");
	}

	//  TODO: process quantization tables
	//  the last half of which is the destination identifier of the 
	//  table (0-3), for instance 0 for the Y component and 1 for the colour components. Next follow the 64 numbers of the table (bytes).
	//
	//		DQT     0xFF, 0xDB	variable size	Define Quantization Table(s)	Specifies one or more quantization tables.
	// 	    DQT                         Which Quantization table (8x8)
	//      ff db : (67 - 00 43 | 67)   00      02 01 01 02 01 01 02 02 02 02 02 02 02 02 03 05 03 03 03 03 03 06 04 04 03 05 07 06 07 07 07 06 07 07 08 09 0b 09 08 08 0a 08 07 07 0a 0d 0a 0a 0b 0c 0c 0c 0c 07 09 0e 0f 0d 0c 0e 0b 0c 0c 0c
	//      ff db : (67 - 00 43 | 67)   01      02 02 02 03 03 03 06 03 03 06 0c 08 07 08 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c
	//      ff db : (67 - 00 43 | 67)   00      02 01 01 02 01 01 02 02 02 02 02 02 02 02 03 05 03 03 03 03 03 06 04 04 03 05 07 06 07 07 07 06 07 07 08 09 0b 09 08 08 0a 08 07 07 0a 0d 0a 0a 0b 0c 0c 0c 0c 07 09 0e 0f 0d 0c 0e 0b 0c 0c 0c
	//      ff db : (67 - 00 43 | 67)   01      02 02 02 03 03 03 06 03 03 06 0c 08 07 08 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c
}

void JpegImage::ProcessSf(const  Segment& segment)
{
	if (segment.data[0] != 8)
	{
		throw RuntimeException("Unsupported precision: " + std::to_string((int)segment.data[0]) + "!");
	}

	if (segment.data[5] != 3)
	{
		throw RuntimeException("Unsupported component count: " + std::to_string((int)segment.data[5]) + "!");
	}

	this->width  = ((int)segment.data[1] << 8 | (int)segment.data[2]);
	this->height = ((int)segment.data[3] << 8 | (int)segment.data[4]);

	// TODO: handle component subsampling 3bytes
	// 6,7,8
	// 9,10,11
	// 12,13,14

	// TODO: baseline vs progresive dct

//	std::cout << "  Img Height: " << std::dec << this->height <<" " << std::setw(2) << std::setfill('0') << std::hex << (int)segment.data[1] << " " << (int)segment.data[2] << std::endl;
//	std::cout << "  Img Width:  " << std::dec << this->width << " " << std::setw(2) << std::setfill('0') << std::hex << (int)segment.data[3] << " " << (int)segment.data[4] << std::endl;

//                Precision Height  Width   #comp Y           Cb          Cr
//	ff c0, 15(15): 08        00 64   00 64   03    01 22 00    02 11 01    03 11 01
//  ff c0, 15(15): 08        00 0a   00 0a   03    01 22 00    02 11 01    03 11 01
//  ff c2, 15(15): 08        04 38   04 38   03    01 22 00    02 11 01    03 11 01
//  ff c2, 15(15): 08        04 a9   04 74   03    01 22 00    02 11 01    03 11 01
}

void JpegImage::ProcessSos(const Segment& segment)
{
//	Begins a top - to - bottom scan of the image.In baseline DCT JPEG images, there is generally a single scan.
//	Progressive DCT JPEG images usually contain multiple scans.This marker specifies which slice of data it will 
//	contain, and is immediately followed by entropy - coded data.

//  ff da   00 0c   03 01 00 02 11 03 11 00 3f 00 fd 68 a2 8a 2b fc 13 3f 68 3f
//	ff da   00 0c   03 01 00 02 11 03 11 00 3f 00 fd 68 a2 8a 2b fc 13 3f 68 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 3f
}

void JpegImage::ProcessDht(const Segment& segment)
{


}

/*

APP0 ff e0, 14(14) : 4a 46 49 46 00 01 01 01 00 60 00 60 00 00
DQT  ff db, 65(65) : 00 02 01 01 02 01 01 02 02 02 02 02 02 02 02 03 05 03 03 03 03 03 06 04 04 03 05 07 06 07 07 07 06 07 07 08 09 0b 09 08 08 0a 08 07 07 0a 0d 0a 0a 0b 0c 0c 0c 0c 07 09 0e 0f 0d 0c 0e 0b 0c 0c 0c
DQT  ff db, 65(65) : 01 02 02 02 03 03 03 06 03 03 06 0c 08 07 08 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c 0c
SF0  ff c0, 15(15) : 08 00 0a 00 0a 03 01 22 00 02 11 01 03 11 01
DHT  ff c4, 29(29) : 00 00 01 05 01 01 01 01 01 01 00 00 00 00 00 00 00 00 01 02 03 04 05 06 07 08 09 0a 0b
DHT  ff c4, 179(179) : 10 00 02 01 03 03 02 04 03 05 05 04 04 00 00 01 7d 01 02 03 00 04 11 05 12 21 31 41 06 13 51 61 07 22 71 14 32 81 91 a1 08 23 42 b1 c1 15 52 d1 f0 24 33 62 72 82 09 0a 16 17 18 19 1a 25 26 27 28 29 2a 34 35 36 37 38 39 3a 43 44 45 46 47 48 49 4a 53 54 55 56 57 58 59 5a 63 64 65 66 67 68 69 6a 73 74 75 76 77 78 79 7a 83 84 85 86 87 88 89 8a 92 93 94 95 96 97 98 99 9a a2 a3 a4 a5 a6 a7 a8 a9 aa b2 b3 b4 b5 b6 b7 b8 b9 ba c2 c3 c4 c5 c6 c7 c8 c9 ca d2 d3 d4 d5 d6 d7 d8 d9 da e1 e2 e3 e4 e5 e6 e7 e8 e9 ea f1 f2 f3 f4 f5 f6 f7 f8 f9 fa
DHT  ff c4, 29(29) : 01 00 03 01 01 01 01 01 01 01 01 01 00 00 00 00 00 00 01 02 03 04 05 06 07 08 09 0a 0b
DHT  ff c4, 179(179) : 11 00 02 01 02 04 04 03 04 07 05 04 04 00 01 02 77 00 01 02 03 11 04 05 21 31 06 12 41 51 07 61 71 13 22 32 81 08 14 42 91 a1 b1 c1 09 23 33 52 f0 15 62 72 d1 0a 16 24 34 e1 25 f1 17 18 19 1a 26 27 28 29 2a 35 36 37 38 39 3a 43 44 45 46 47 48 49 4a 53 54 55 56 57 58 59 5a 63 64 65 66 67 68 69 6a 73 74 75 76 77 78 79 7a 82 83 84 85 86 87 88 89 8a 92 93 94 95 96 97 98 99 9a a2 a3 a4 a5 a6 a7 a8 a9 aa b2 b3 b4 b5 b6 b7 b8 b9 ba c2 c3 c4 c5 c6 c7 c8 c9 ca d2 d3 d4 d5 d6 d7 d8 d9 da e2 e3 e4 e5 e6 e7 e8 e9 ea f2 f3 f4 f5 f6 f7 f8 f9 fa
SOS  ff da, 10(20) : 03 01 00 02 11 03 11 00 3f 00 fd 68 a2 8a 2b fc 13 3f 68 3f

*/