#include "jpg/JpegImage.hpp"
#include "Exception.hpp"

#include <fstream>
#include <sstream>
#include <cstdint>
#include <iomanip>

// TMP ////////////////////////////////////////////////////
#include <iostream>
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
					// If 0x00 0xff - byte is part of data stream not a marker!
					// TODO check if only SOS ????
					if (segment.data[segment.data.size()-1] == 0x00)
					{
std::cout << "escaped 0xff" << std::endl;
						segment.data[segment.data.size() - 1] = 0xff;
						continue;
					}
					else
					{
						ProcessSegment(segment);
					}
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
}

void JpegImage::ProcessSegment(const Segment& segment)
{
//	LogSegment(segment);

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
		break;
	default:
		// Ignore unknown segments
		break;
//		throw RuntimeException("Unsupported segment " + std::to_string((int)segment.marker[1]) + "!");
	}
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
	std::cout << "----------------------------------------------" << std::endl;
	LogSegment(segment);
	for (auto x : segment.data)
	{
		for (int i = 0; i < 8; i++)
		{
			std::cout << ((x & 1 << i) ? "1" : "0");
		}
	}
	std::cout << std::endl << "----------------------------------------------" << std::endl;

	// ff da, 10(20): 
	// 03 01 00 02 11 03 11 00 3f 00 fd 68 a2 8a 2b fc 13 3f 68 3f
	// 1100000010000000000000000100000010001000110000001000100000000000111111000000000010111111000101100100010101010001110101000011111111001000111111000001011011111100

//	Begins a top - to - bottom scan of the image.In baseline DCT JPEG images, there is generally a single scan.
//	Progressive DCT JPEG images usually contain multiple scans.This marker specifies which slice of data it will 
//	contain, and is immediately followed by entropy - coded data.

//  ff da   00 0c   03 01 00 02 11 03 11 00 3f 00 fd 68 a2 8a 2b fc 13 3f 68 3f
//	ff da   00 0c   03 01 00 02 11 03 11 00 3f 00 fd 68 a2 8a 2b fc 13 3f 68 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 0a 28 a2 80 3f

//	ff da, 10(309) : 03 01 00 02 10 03 10 00 00 00 fb cc a8 7d 2f 3d 8a 8a 5d 82 98 16 0a d6 8b 45 50 1e 56 b2 5a 88 b4 59 29 25 38 ad 59 72 20 0b 05 6e 56 5c ba f2 27 d1 47 4b b7 ba 9b 92 45 b1 54 86 57 0b 08 2d 25 76 57 a5 88 65 5c 32 0a 30 83 86 08 98 64 06 10 80 92 30 90 00 90 42 40 68 90 12 8c 04 40 c9 2e 00 28 63 5b 94 40 24 90 99 15 6b 7c 1a 6a 6e 20 22 d2 8f 21 90 2a 32 10 00 c0 06 40 90 40 8c 08 18 ac 94 2a c3 58 48 84 20 70 c2 10 10 49 90 0c 83 07 20 80 60 80 1a 00 32 40 58 49 02 40 43 44 28 04 94 80 65 a5 22 84 dd 44 52 09 84 82 08 c2 94 25 c1 20 82 14 05 60 12 48 11 5d 40 43 08 dc 2e 19 ba 53 55 65 52 2d 21 98 68 ad 88 97 84 52 2e 05 50 b7 ab 2a 5d 0a 14 4b 63 aa 53 42 39 a4 ba 55 67 cd 7d 12 fa 68 e9 4e cb 69 b4 70 18 a1 5d 58 56 10 c2 4a ac ab 4d 4a 93 76 41 82 90 c9 08 91 28 64 1c 91 da 48 60 c1 04 24 26 a4 03 07 01 05 10 b5 a2 c1 9f 9e cd 7c fe 47 9f 2f d1 73 fc 97 2a 0f a2 76 bf 39 d1 9c 7e a0 e5 fe 6b f4 da 4f da fa
//	ff da, 6(108)  : 01 02 01 01 3f 00 f8 a8 8a 23 d9 2d 91 df 78 4f e0 4a c6 b0 f7 bc 5f c8 d1 78 7f 25 b2 f1 65 96 26 58 99 62 62 1e 51 1c de 2f 35 b2 b1 74 4f 55 24 7f 3d 2e 51 0d 78 31 6a c1 fd 9e f1 6f b3 d2 f9 47 a0 d6 cb f9 ef 73 dc fe 58 88 7b 23 87 9a 2b 6c 51 c1 39 d2 b6 3d 5b e8 f7 e7 93 53 51 57 02 6c 53 6f 81 cd c5 9a 3e 5b 8f 67
//	ff da, 6(166)  : 01 03 01 01 3f 00 dd 5b 59 02 42 65 e1 2c 57 23 dd c0 b0 b6 5e c4 f7 bc 5e c5 b9 ba c3 78 a2 c6 f2 df d6 8a db 45 10 24 32 f0 8b 39 b2 c4 8a c5 14 59 1c a4 25 ed 95 cf 24 b9 e9 97 fb 1b fd 0d 70 26 37 5d 97 b6 f7 de d9 63 92 cb cd 8b 91 ad 8f 14 fe b4 32 24 b6 45 62 8a e0 4c b1 bc fc 45 8a 23 12 6d 70 6a 36 fa 22 f9 e4 6c 53 ae 4f 9d a1 2b 20 fd 32 22 7f 7a 25 8b 19 7b 11 79 78 65 62 8a c5 15 bd 31 ec 8b df 45 1d f0 8a 7e 8f c9 72 d1 7d 31 ca d7 03 e5 0c b3 fe 12 23 c2 13 1c 99 d7 28 8c 91 f2
//	ff da, 6(8)    : 01 01 00 01 3f 02 f4 cf
//	ff da, 6(752)  : 01 01 00 01 0a 21 bc 5e 6f 85 e2 f8 5f 82 43 a8 c3 ea ee 5c bf f8 8b 97 8b 97 8b 97 2f 17 9b 97 2e 5e 6e 5e 67 73 a4 61 9a fa 0e b1 66 17 2f 0b 51 92 04 f0 f5 f4 6c bf 33 e5 af 83 5c 5c 21 d6 0f 03 f2 af f2 6e 6d 16 25 68 2a 14 65 52 d3 20 ad 0f 6c 05 83 05 c4 9a 78 95 cb 5f 6e 99 49 59 ac d6 6a 56 6a 56 27 70 ea 30 e5 59 7c 07 9e c9 d2 e5 bd e3 17 c1 2b 79 86 09 61 ba 43 69 6a 89 25 5a 96 0a 34 ee 4a a1 2e 8b 2e d7 3b 47 1f 16 9d fd bd 7c 4b f1 98 43 ac 1e 07 e3 db 0d 6d 47 7d 62 09 7d 82 09 e8 48 d9 51 87 57 1a 85 c6 56 25 ab ac 1e 82 9c 9d 8a b9 66 25 36 66 06 ea b2 d0 31 26 b2 e7 7f f0 af c1 90 eb 07 81 f8 d5 15 0e c6 5d b8 26 d9 b6 94 20 4a d6 10 40 ef 8b 65 c3 74 45 6c 8f 75 2b 2a 1f eb 6a ae d5 64 65 dc 7b 9f ce 00 5e 17 0a 69 78 02 cc 1b eb fe 02 f3 79 bf 0d f2 90 eb 07 cc fe cb 3d c0 96 91 ca 56 d1 ef 42 8d 46 55 bb 3b dd c7 d2 6e c5 7b 80 bd 20 f6 51 e8 90 a6 ee 1d 15 03 78 e6 8c 3f 90 7d e0 c5 b1 81 21 94 b5 7f 52 af ac 90 ea 30 e2 7c 6b 5f 2a 0a 7a 35 ac b4 dd c9 1c 6b 53 b9 67 68 b7 9b 96 42 1f 92 e9 88 76 4b d5 31 64 ac f5 08 dd b8 9f 90 02 37 77 12 12 00 9a f0 2f fe 4a 43 a8 c3 0e 48 7c 3f ec b7 c9 74 49 47 71 62 cb 31 61 b8 e0 ec cd d9 be 55 10 21 09 54 2a bb c0 2f de 9b 22 f8 8b 14 a1 d2 ee a0 1a 13 b0 c4 8d 90 4d cb 48 c3 fe 4d 49 d3 07 13 2f c0 33 69 67 78 e2 ea 0a 82 41 d4 1a 8e dc 18 30 49 bf 55 fe 4d 7b 62 92 e1 ed 0e 84 84 54 5e e1 f8 61 54 61 eb 1a 69 14 f6 2b 49 5e d9 be ac 68 6c 07 62 8b 29 c4 bb ea 5c 43 ee 5f 8f 21 d6 0e 26 5f 3a c0 ca 94 53 46 e9 68 86 db 42 7a 96 e0 9b 9e f4 fc c6 b3 a8 a2 2e 7d c0 3b 21 77 14 2e 1a 6c fb f9 f8 25 e2 5a 61 65 b8 6d a1 88 7d d3 b4 d4 6d aa 70 cd 71 7d da be 2c ee 74 8c 38 9f 04 0e e2 40 4a 59 80 ca cd 31 6d 4a 5b dc a9 50 4e 63 56 aa 7a 89 2a 6f 82 54 10 db 61 4d 84 50 85 eb b4 a4 6b 02 67 70 34 ee 1a f7 1d 3b 96 0e c0 ed 3f 91 24 ef ba 61 09 18 90 4e c6 10 7f c7 a7 51 86 5f 3d f0 dc 23 2e 3a 2b 95 1b 0e c7 0e e6 f6 a2 f6 31 95 a0 5d 4b 1a 60 d4 6a 2c b7 85 c0 be e7 ba 2d 0e 97 0d 17 17 69 65 72 f1 53 b4 2f a8 0e 98 e1 a4 c6 d4 81 46 41 4f f8 f9 21 d4 61 c4 cb e6 37 60 bc 11 e1 ab 87 f9 36 19 de 89 58 10 72 89 48 f0 da 63 12 71 80 7a 8c 4d e5 36 b8 35 12 dd b3 d4 d0 45 1b c4 0c bc 9a 6a 74 cb fc 86 f7 06 e2 4e 9b 8b 37 02 20 aa 52 c5 29 35 15 74 87 a5 c1
//	ff da, 6(6)    : 01 01 00 0b 3f 21
//	ff da, 6(99)   : 01 01 00 01 0a 10 e9 e4 f1 91 9e ed d5 2d f7 1d 64 a6 33 6e 72 46 df 36 f4 d9 4d b4 4d b7 24 61 7b 52 1f 90 f7 b2 99 3f b2 1b 1d 6f 7b d7 7d 5b b3 67 ad e9 ed f7 90 fe 5f ec 7b 9e 4c 19 e0 fc 63 85 cc be 5f e7 8e c4 9f 2c bb 9c 92 f7 e1 f1 df 1b ea f7 7a b7 e5 bf 63 26 ee 58 64 06 f6 70 49 7a e5
//	ff da, 6(408)  : 01 01 00 0b 13 10 cf dd 27 68 de 75 bf 33 f1 2f 17 fb 88 16 b7 ad e5 a7 88 d9 6d 5a de f2 8a d7 f0 9d 23 13 a4 6f 78 af 0a de fc 17 e4 7f 4b da bc 35 b3 c5 6a b7 fb 9f f2 f5 86 59 de b7 ac 95 97 23 8c f3 10 38 f3 49 78 eb 79 7e 7b de bf a2 72 ca ca c7 bc c6 93 e0 e7 46 f4 fd d1 6a 86 f8 ca 7c 9c d1 d0 9d 15 ed a0 6f 81 5c a3 f5 49 58 81 1f a6 e0 6c 93 9c 5d 92 f0 85 07 dd ab 35 8f f5 97 a4 72 5e 1e 1c 68 a3 7b 1e 63 48 f0 2f ed 5f c5 18 8d 7b cc 62 bc 90 42 24 3e 08 e0 97 41 bb 9c 8b 10 87 a4 45 52 14 59 27 04 11 58 5b b6 9a f2 11 a6 32 44 b6 84 86 f6 b7 9c d1 3a 37 e6 9f 04 63 af 0a 62 c3 f2 7c 17 82 33 24 f9 56 df 7c 1f 33 f7 c5 59 bc a2 0a 62 92 e5 8b 93 94 dc 3e a9 7b 93 8b 94 e7 21 89 b6 d4 12 34 b8 4d 37 49 a6 56 d2 f7 21 93 14 8d 1d 09 74 0d a8 17 b1 8b 3f 74 7e 2f 9f c1 5e 5f cd 2f 5f 78 9c 7a 8f 2d e6 bc 77 89 7f c7 0b c6 d8 e7 d3 b1 21 7e b0 29 50 d0 92 a5 52 62 1a 49 10 b4 be 48 fb 02 39 29 14 23 dc af a4 f2 22 e4 41 01 4b 04 9c b1 ec b3 f7 30 4e 23 0f c5 f7 f9 af 4a dd bc a9 cc 79 ec 82 71 1a 4e d4 56 88 8f 2d 78 28 50 47 02 8b 48 a5 31 06 39 96 e3 48 3f 20 e3 07 1e c8 43 68 aa 80 e0 cb 84 be dc f2 34 c9 13 94 d9 3c 57 25 fb 6e 44 f0 89 71 12 e4 75 e5 9c c6 9d 79 2f 59 f1 4e 63 4f be 2e 31 04
//	ff da, 6(144)  : 01 01 00 14 3f 10 e2 d4 e7 7f 5e 87 5e 51 3f 66 17 bb bd 95 48 3c 22 e0 72 11 df a7 a3 5b 89 4d 9f 95 00 f3 f2 8b c9 cb 88 08 02 c5 ea 01 f9 50 72 89 e0 a9 3e 84 85 07 be 8a 24 14 64 aa 8f b9 54 7d d4 94 51 e7 75 2a 41 a6 96 46 0e fe aa 01 61 72 f9 0d 15 d4 81 ca 2f 07 76 6e 57 e9 90 7f 84 1d 1a 1e e8 97 39 47 a9 a3 00 32 ae 39 75 f6 6b a3 f4 ca 9e c8 c8 99 21 70 8b 01 9a 97 72 93 ac a0 aa 8b 83 02 30 55 ca 82 83 2e 84 5d 65 02
}

void JpegImage::ProcessDht(const Segment& segment)
{
	// last three bits has to be 0
	if (segment.data[0] >> 5)
	{
		throw RuntimeException("Invalid DHT data");
	}

	// htNumber = first half of byte
	auto htNumber = segment.data[0] & 0x0f;
	if (htNumber > 3)
	{
		throw RuntimeException("Invalid htNumber "+std::to_string(htNumber));
	}

	// type of HT, 0 = DC table, 1 = AC table
	auto htType = segment.data[0] >> 4;

	// list of code lengths and codes
	std::vector<HuffmanCode> codes;
	uint32_t offset = 17; // index of next code
	for (uint8_t i = 1; i <= 16; i++)
	{
		// Add all codes of length i to the list
		for (uint8_t j = 0; j < segment.data[i]; j++)
		{
			// value, code (0 - to be assigned), length
			HuffmanCode entry = { segment.data[offset++], 0, i };
			codes.push_back(entry);
		}
	}
	
	// Generate codes for code / lengths
	HuffmanCode::AsignCodes(codes);

	// DC table
	if (htType == 0)
	{
		this->dhtDCTables[htNumber] = codes;
	}

	// AC table
	if (htType == 0)
	{
		this->dhtACTables[htNumber] = codes;
	}
}
