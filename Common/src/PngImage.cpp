#include "PngImage.hpp"
#include "Pixel.hpp"
#include "Exception.hpp"
#include "Common.hpp"
#include "BitStream.hpp"

#include <fstream>
#include <memory>

#include <iostream>
#include <cstdint>
#include <iomanip>
#include <bitset>

#define TO_INT(data)  ((data[0] << 8 * 3) | (data[1] << 8 * 2) | (data[2] << 8 * 1) | data[3])

// Info Header
PACK(struct IHDR
{
	uint8_t width[4];				// image width (note: in little endian, invert byteorder for value)
	uint8_t height[4];				// image height (note: in little endian, invert byteorder for value)
	uint8_t chanellSize;			// number of bytes per chanel per pixel
	uint8_t colorType;				// 2 = RGB/truecolor, TODO other vals
	uint8_t compressionMethod;		// 0
	uint8_t filterMethod;			// 0
	uint8_t interfaceMethod;		// 0 - no interface 1 - ADAM7
});

enum class ColorType : uint8_t
{
	GrayScale      = 0b000,
	TrueColor      = 0b010, // RGB
	Indexed        = 0b011,
	GrayScaleAlpha = 0b100,
	TrueColorAlpha = 0b110 // RGBA
};

PngImage::PngImage(const std::string& filename)
{
	LoadData(filename);
}

void PngImage::LoadData(const std::string& filename)
{
	auto chunks = LoadChnuks(filename);

	// Process header chunk (ihdr)
	IHDR ihdr = { 0 };
	auto ihdrChunk = std::find_if(chunks.begin(), chunks.end(),
		[](const Chunk& chunk) { return chunk.header.chunkType == "IHDR"; });
	if (ihdrChunk == chunks.end())
	{
		throw RuntimeException("Missing IHDR chunk!");
	}
	std::memcpy(&ihdr, ihdrChunk->data.get(), sizeof(IHDR));

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	std::cout << ihdrChunk->header.chunkType << std::endl;/////////////////////////////////////////////////////////////////////////////////////////
	std::cout << "- width:             ";		///////////////////////////////////////////////////////////////////////////////////////////////////
	for (int i = 0; i < 4; i++)		///////////////////////////////////////////////////////////////////////////////////////////////////////////////
		std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)ihdr.width[i] << " ";		///////////////////////////////////////////
	std::cout << " (" << std::dec << (int)(TO_INT(ihdr.width)) << ") ";		///////////////////////////////////////////////////////////////////////
	std::cout << " [" << std::dec << (int)(this->width) << "]" << std::endl;		///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	std::cout << "- height:            ";		///////////////////////////////////////////////////////////////////////////////////////////////////
	for (int i = 0; i < 4; i++)		///////////////////////////////////////////////////////////////////////////////////////////////////////////////
		std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)ihdr.height[i] << " ";		///////////////////////////////////////////
	std::cout << " (" << std::dec << (int)(TO_INT(ihdr.height)) << ") ";		///////////////////////////////////////////////////////////////////
	std::cout << " [" << std::dec << (int)(this->height) << "]" << std::endl;		///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	std::cout << "- chanellSize:       " << std::setw(2) << std::setfill('0') << std::hex << (int)ihdr.chanellSize << std::endl;		///////////
	std::cout << "- colorType:         " << std::setw(2) << std::setfill('0') << std::hex << (int)ihdr.colorType << std::endl;		///////////////
	std::cout << "- compressionMethod: " << std::setw(2) << std::setfill('0') << std::hex << (int)ihdr.compressionMethod << std::endl;		///////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Check unsupported types
	// TODO: implement other color types
	// NOTE: only RGB & RGBA supported
	if (static_cast<ColorType>(ihdr.colorType) != ColorType::TrueColor &&
		static_cast<ColorType>(ihdr.colorType) != ColorType::TrueColorAlpha)
	{
		throw RuntimeException("Unsupported color type: \"" + std::to_string(ihdr.colorType) + "\"");
	}

	// NOTE: ADAM7 not supported
	if (ihdr.interfaceMethod != 0)
	{
		throw RuntimeException("Unsupported interface method: \"" + std::to_string(ihdr.interfaceMethod) + "\"");
	}

	// Only LZ77 is supported by png
	// TODOCheck compression method to be 0 

	// Allocate image data
	this->width = (TO_INT(ihdr.width));
	this->height = (TO_INT(ihdr.height));
	this->image = std::unique_ptr<std::unique_ptr<Pixel>[]>(new std::unique_ptr<Pixel>[this->width * this->height]);

	// TODO Process other chunks if needed

	// Merge all IDAT chunks
	BitStream bitstream;
	for (auto& c : chunks)
	{
		if (c.header.chunkType != "IDAT")
		{
			continue;
		}
		bitstream.Append(c.data.get(), c.header.lenght);

		///////////////////////////////////////////////////////////////////////////////////////////////
		std::cout << "------------------------------------------------------------" << std::endl;//////
		std::cout << "IDAT" << std::endl;//////////////////////////////////////////////////////////////
		for (int i = 0; i < c.header.lenght; i++)		///////////////////////////////////////////////
		{		///////////////////////////////////////////////////////////////////////////////////////
			auto byte = c.data[i];		///////////////////////////////////////////////////////////////
			for (int j = 0; j < 8; j++)		///////////////////////////////////////////////////////////
			{		///////////////////////////////////////////////////////////////////////////////////
				auto bit = byte & 1 << j;		///////////////////////////////////////////////////////
				std::cout << (bit > 0 ? "1" : "0");		///////////////////////////////////////////////
			}		///////////////////////////////////////////////////////////////////////////////////
			//std::cout << "  ";		///////////////////////////////////////////////////////////////////
			//if ((i % 5) == 4)		///////////////////////////////////////////////////////////////////
			//	std::cout << std::endl;		///////////////////////////////////////////////////////////
		}		///////////////////////////////////////////////////////////////////////////////////////
		std::cout << std::endl;////////////////////////////////////////////////////////////////////////
		std::cout << "------------------------------------------------------------" << std::endl;//////
		///////////////////////////////////////////////////////////////////////////////////////////////
	}

	auto idatHdr1 = bitstream.GetCurrentByte();
	auto idatHdr2 = bitstream.GetCurrentByte();

	// png supports ony compression method - 0x08 "deflate"
	auto compressionMethod = idatHdr1 & 0x0F;
	if (compressionMethod != 0x08)
	{
		throw RuntimeException("Unsupported compression method: \"" + std::to_string(compressionMethod) + "\"");
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	std::cout << "- data 0    " << std::setw(2) << std::setfill('0') << std::hex << (int)idatHdr1 << " (" << std::bitset<8>(idatHdr1) << ")" << std::endl;////
	std::cout << "- data 1    " << std::setw(2) << std::setfill('0') << std::hex << (int)idatHdr2 << " (" << std::bitset<8>(idatHdr2) << ")" << std::endl;////
	std::cout << "   - Compression method " << std::setw(2) << std::setfill('0') << std::dec << (int)compressionMethod << std::endl;//////////////////////////
	std::cout << "   - Compression info   " << std::setw(2) << std::setfill('0') << std::dec << (int)(idatHdr1 >> 4) << std::endl;////////////////////////////
	std::cout << "   - F Check            " << std::setw(2) << std::setfill('0') << std::dec << (int)(idatHdr2 & 0x1f) << std::endl;//////////////////////////
	std::cout << "   - F DICT             " << std::setw(2) << std::setfill('0') << std::dec << (int)((idatHdr2 >> 5) & 0x1) << std::endl;////////////////////
	std::cout << "   - F Level            " << std::setw(2) << std::setfill('0') << std::dec << (int)(idatHdr2 >> 6) << std::endl;////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	std::vector<uint32_t> data;

	bool isLast = false;
	do
	{
		isLast = bitstream.GetNext();
		auto typeBit1 = bitstream.GetNext();
		auto typeBit2 = bitstream.GetNext();

		// TODO verify order of bites!!!
		if (!typeBit2 && !typeBit1) // 00
		{
			std::cout << "No Compression" << std::endl;
		}
		else if (!typeBit2 && typeBit1) // 01
		{
			std::cout << "Compressed with fixed codes" << std::endl;

			while (true)
			{
				auto bit6 = bitstream.GetNext();
				auto bit5 = bitstream.GetNext();
				auto bit4 = bitstream.GetNext();
				auto bit3 = bitstream.GetNext();
				auto bit2 = bitstream.GetNext();
				auto bit1 = bitstream.GetNext();
				auto bit0 = bitstream.GetNext();

				/*
				auto bit0 = bitstream.GetNext();
				auto bit1 = bitstream.GetNext();
				auto bit2 = bitstream.GetNext();
				auto bit3 = bitstream.GetNext();
				auto bit4 = bitstream.GetNext();
				auto bit5 = bitstream.GetNext();
				auto bit6 = bitstream.GetNext();//*/

				uint32_t num = (int)bit0 | ((int)bit1 << 1) | ((int)bit2 << 2) | ((int)bit3 << 3) | ((int)bit4 << 4) | ((int)bit5 << 5) | ((int)bit6 << 6);
//				std::cout << "????????? " << std::setw(3) << std::setfill('0') << std::dec << num << "     " << bit6 << bit5 << bit4 << bit3 << bit2 << bit1 << bit0 << std::endl;
				// 7bit
				if (/*!bit6 && !bit5 && */(num <= 23)) // 7bit: 256 - 279
				{
					num += 256;
					std::cout << std::setw(3) << std::setfill('0') << std::dec << num << "     " << bit6 << bit5 << bit4 << bit3 << bit2 << bit1 << bit0 << std::endl;
					if (num == 256)
					{
						break;
					}
				}
				else
				{
					// 8bit
					auto bit7 = bitstream.GetNext();
					num = (num << 1) | ((int)bit7);
//					num = num | ((int)bit7 << 7);
					if ((num >= 48) && (num <= 191)) // 0 - 143
					{
						num -= 48;
						std::cout << std::setw(3) << std::setfill('0') << std::dec << num << "    " << bit7 << bit6 << bit5 << bit4 << bit3 << bit2 << bit1 << bit0 << std::endl;
					}
					else if ((num >= 192) && (num <= 199)) // 280 - 287
					{
						num -= 88;
						std::cout << std::setw(3) << std::setfill('0') << std::dec << num << "    " << bit7 << bit6 << bit5 << bit4 << bit3 << bit2 << bit1 << bit0 << std::endl;
					}
					else
					{
						// 9bit 144 - 255
						auto bit8 = bitstream.GetNext();
						num = (num << 1) | ((int)bit8);
//						num = num | ((int)bit8 << 8);
//						std::cout << "???????????????????????????????? " << std::setw(3) << std::setfill('0') << std::dec << num << "   " << bit8 << bit7 << bit6 << bit5 << bit4 << bit3 << bit2 << bit1 << bit0 << std::endl;
						num -= 256;
						std::cout << std::setw(3) << std::setfill('0') << std::dec << num << "   " << bit8 << bit7 << bit6 << bit5 << bit4 << bit3 << bit2 << bit1 << bit0 << std::endl;
					}
				}

				if ((num == 286) || (num == 287))
				{
					throw RuntimeException("Invalid length code " + std::to_string(num));
				}

				// 257..285
				if ((num >= 257) && (num <= 285))
				{
					// handle lengths
					// num == length code
					int extra = 0;
					if (num >= 265 && num <= 268) // 1 extra bit
					{
						extra = bitstream.GetNext();
					}
					else if (num >= 269 && num <= 272) // 2 extra bits
					{
						auto ex0 = bitstream.GetNext();
						auto ex1 = bitstream.GetNext();
						extra = (int)ex0 | ((int)ex1 << 1);
					}
					else if (num >= 273 && num <= 276)  // 3 extra bits
					{
						auto ex0 = bitstream.GetNext();
						auto ex1 = bitstream.GetNext();
						auto ex2 = bitstream.GetNext();
						extra = (int)ex0 | ((int)ex1 << 1) | ((int)ex2 << 2);
					}
					else if (num >= 277 && num <= 280) // 4 extra bits
					{
						auto ex0 = bitstream.GetNext();
						auto ex1 = bitstream.GetNext();
						auto ex2 = bitstream.GetNext();
						auto ex3 = bitstream.GetNext();
						extra = (int)ex0 | ((int)ex1 << 1) | ((int)ex2 << 2) | ((int)ex3 << 3);
					}
					else if (num >= 281 && num <= 285) // 5 extra bits
					{
						auto ex0 = bitstream.GetNext();
						auto ex1 = bitstream.GetNext();
						auto ex2 = bitstream.GetNext();
						auto ex3 = bitstream.GetNext();
						auto ex4 = bitstream.GetNext();
						extra = (int)ex0 | ((int)ex1 << 1) | ((int)ex2 << 2) | ((int)ex3 << 3) | ((int)ex4 << 4);
					}


					auto distBit4 = bitstream.GetNext();
					auto distBit3 = bitstream.GetNext();
					auto distBit2 = bitstream.GetNext();
					auto distBit1 = bitstream.GetNext();
					auto distBit0 = bitstream.GetNext();
					/*auto distBit0 = bitstream.GetNext();
					auto distBit1 = bitstream.GetNext();
					auto distBit2 = bitstream.GetNext();
					auto distBit3 = bitstream.GetNext();
					auto distBit4 = bitstream.GetNext();*/


					uint32_t distCode = (int)distBit0 | ((int)distBit1 << 1) | ((int)distBit2 << 2) | ((int)distBit3 << 3) | ((int)distBit4 << 4);
					std::cout << "extra: " << extra << ", distCode: " << distCode << std::endl;

					if ((distCode == 30) || (distCode == 31))
					{
						throw RuntimeException("Invalid distance code " + std::to_string(distCode));
					}

					if (distCode > 3)
					{
						auto c = (distCode - 2) / 2;
						std::cout << "? " << distCode << ": " << c << std::endl;
						for (auto k = 0; k < c; k++)
						{
							bitstream.GetNext();
						}
					}

// 00011000111010101100011000011011100010100001100000001111111110011000011110110101001100010101001111111110011000001010010110110001011111001111111110011001011001011011100000000110100001100000011000001101111011110001110011001111111110000100010010001111111111111100100011000011111111100000000000010000100001111111000010011011

//			00011000  11101010  // Header
//								1
//								 10
//								   00110  00011011  10001010
//			00011000  00001111  11111000  00000000  
//			10100000  11101011  01000000  10011110 // check value

//			00011000  11101010  // Header
//								1 // last
//								 10 
//								   00110  00011111  11111111
//			10010001  10000111  11111100  11000011  11011010
//			10011000  10101001  11111111  00110000  01101111
//			01111000  11100110  01111111  11000000  00000000
//			10011100  01101011  11100000  11110110 // check value


/*					auto start = 
					for (int i = dist; i >= 0; i--)
					{
						auto val = data.size() - 1 - i;
						data.p
					}*/

//					std::cout << "decode distance from input stream move backwards distance bytes in the output"
//						<< " stream, and copy length bytes from this position to the output stream." << std::endl;
				}
				else
				{
					// handle literal
					data.push_back(num);
				}

			}
		}
		else if (typeBit2 && !typeBit1) // 10
		{
			std::cout << "compressed with dynamic Huffman codes" << std::endl;
			// TODO


		}
		else if (!typeBit2 && typeBit1) // 11
		{
			throw RuntimeException("Invalid DEFLATE type specifier");
		}
	} while (!isLast);


	// TODO


	/*//////////////////////////////////////////////////////////////////////////////////////////////
	std::cout << std::endl;	///////////////////////////////////////////////////////////////////////
	for (int i = 0; i < 20; i++)	///////////////////////////////////////////////////////////////
	{	///////////////////////////////////////////////////////////////////////////////////////////
		for (int j = 0; j < 8; j++)	///////////////////////////////////////////////////////////////
		{	///////////////////////////////////////////////////////////////////////////////////////
			auto bit = bitstream.GetNext();	///////////////////////////////////////////////////////
			std::cout << bit;	///////////////////////////////////////////////////////////////////
		}	///////////////////////////////////////////////////////////////////////////////////////
		std::cout << "  ";	///////////////////////////////////////////////////////////////////////
		if ((i % 5) == 4)	///////////////////////////////////////////////////////////////////////
			std::cout << std::endl;	///////////////////////////////////////////////////////////////
	}	///////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////*/

}

std::vector<Chunk> PngImage::LoadChnuks(const std::string& filename)
{
	std::vector<Chunk> chunks;

	// Check file
	std::ifstream file(filename, std::ios::binary);
	if (!file.is_open())
	{
		throw RuntimeException("Unable to open file: \"" + filename + "\"");
	}

	// Check if png data
	uint8_t signature[8];
	file.read((char*)signature, sizeof(signature));
	uint8_t reference[8] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
	if (std::memcmp(signature, reference, 8) != 0)
	{
		throw RuntimeException("Not a png file (" + filename + ")!");
	}

	// Load chnuks
	while (file)
	{
		Chunk chunk;

		// Read  size
		uint8_t data[4];
		file.read((char*)data, 4);
		chunk.header.lenght = TO_INT(data);
		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		std::cout << "Size: " << std::dec << chunk.header.lenght << std::endl;		/////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////

		// Read chunk type
		file.read((char*)data, 4);
		chunk.header.chunkType = std::string((char*)data, 4);
		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		std::cout << "Type: " << chunk.header.chunkType << std::endl;;		/////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////

		// Read data
		chunk.data = std::unique_ptr<uint8_t[]>(new uint8_t[chunk.header.lenght]);
		file.read((char*)chunk.data.get(), chunk.header.lenght);
		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		std::cout << "Data: ";		/////////////////////////////////////////////////////////////////////////////
		for (uint32_t i = 0; i < (chunk.header.lenght < 100 ? chunk.header.lenght : 100); i++)		/////////////
		{		/////////////////////////////////////////////////////////////////////////////////////////////////
			std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)chunk.data[i] << " ";		/////
		}		/////////////////////////////////////////////////////////////////////////////////////////////////
		if (chunk.header.lenght > 100)		/////////////////////////////////////////////////////////////////////
			std::cout << "...";		/////////////////////////////////////////////////////////////////////////////
		std::cout << std::endl;		/////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////

		// Read crc
		file.read((char*)&chunk.crc, 4);
		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		std::cout << "Crc : ";		/////////////////////////////////////////////////////////////////////////////
		for (int i = 0; i < 4; i++)		/////////////////////////////////////////////////////////////////////////
		{		/////////////////////////////////////////////////////////////////////////////////////////////////
			std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)chunk.crc[i] << " ";		/////////
		}		/////////////////////////////////////////////////////////////////////////////////////////////////
		std::cout << std::endl;		/////////////////////////////////////////////////////////////////////////////
		std::cout << std::endl;		/////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////

		// Is last chunk
		if (chunk.header.chunkType == std::string("IEND"))
		{
			chunks.push_back(std::move(chunk)); // append chunks
			break;
		}
		chunks.push_back(std::move(chunk));// append chunks
	}

	file.close();
	return chunks;
}
