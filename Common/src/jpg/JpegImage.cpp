#include "jpg/JpegImage.hpp"
#include "Exception.hpp"

#include <fstream>
#include <sstream>
#include <cstdint>
#include <iomanip>

// TMP ////////////////////////////////////////////////////
#include <iostream>
// TMP ////////////////////////////////////////////////////

static constexpr uint8_t zigZag[8][8] = {{ 0, 1, 5, 6,14,15,27,28},
										 { 2, 4, 7,13,16,26,29,42},
										 { 3, 8,12,17,25,30,41,43},
										 { 9,11,18,24,31,40,44,53},
										 {10,19,23,32,39,45,52,54},
										 {20,22,33,38,46,51,55,60},
										 {21,34,37,47,50,56,59,61},
										 {35,36,48,49,57,58,62,63}};

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
		std::dec << size << "(" << std::setw(2) << std::setfill('0') << std::hex << (int)segment.size[0] << " " << (int)segment.size[1] << "): ";
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
	if(!file.good())
	{
		throw std::runtime_error("Failed to open file " +filename +": "+strerror(errno));
	}

	uint8_t buffer[128];

	// Process start of image
	file.read((char*)&buffer, 2);
	if (buffer[0] != 0xff && buffer[1] != 0xd8)
	{
		throw std::runtime_error("Not jpeg image!");
	}

	// Load segments
	Segment segment;
	while (file.good())
	{
		file.read((char*)&segment.marker, 2);
		if (segment.marker[0] != 0xff)
		{
			std::stringstream ss;
			ss << "Was expecting start of segment marker, instead got " << std::setw(2) << std::setfill('0') << std::hex << (int)segment.marker[0] << "!";
			throw std::runtime_error(ss.str());
		}
		// End Of Image (EOI)
		if (segment.marker[1] == 0xd9)
		{
			break;
		}

		file.read((char*)&segment.size, 2);
		auto size = ((int)segment.size[0] << 8 | (int)segment.size[1]) - 2;
		do
		{
			auto toRead = size < sizeof(buffer) ? size : sizeof(buffer);
			size -= toRead;

			file.read((char*)&buffer, toRead);
			segment.data.insert(segment.data.end(), buffer, buffer + toRead);

		} while (size > 0);

		// Load start of scan (SOS) data
		if (segment.marker[1] == 0xda)
			{
			uint8_t current;
			while (file.good())
				{
				file >> current;
				if (current == 0xff)
					{
					file >> current;
					if (current >= 0xd0 && current <= 0xd0 || current == 0x00)
					{
						// 0xff escaped - add only 0xff
						segment.data.push_back(0xff);
                        // TODO only 00 escapes???
                        if (current != 0x00)
						segment.data.push_back(current);
					}
					else
					{
						// End of SOS, move back in file
						file.seekg(std::ios::cur - 2);
					}
				}
				else
				{
					segment.data.push_back(current);
				}

			}
		}

		// TODO add to vector or process segment
		ProcessSegment(segment);
				segment.data.clear();
	}

			}

void JpegImage::ProcessSegment(const Segment& segment)
{
	std::cout << "****************************************************************************" << std::endl;
	switch (segment.marker[1])
	{
	case 0xe0:
        ProcessApp0(segment);
        break;
	case 0xe1:
        // Skip app1 metadata
		break;
	case 0xdb: // Define Quantization Tables (DQT)
		ProcessDqt(segment);
		break;
	case 0xc0: // "Start of frame" (SOF) - baseline DCT
	case 0xc2: // "Start of frame" (SOF) - progresive DCT
		ProcessSof(segment); // Image metadata
		break;
	case 0xda: // Start of stream (SoS)
		ProcessSos(segment);
		break;
	case 0xc4: // Define Huffman table (DHT)
		ProcessDht(segment);
		break;
	default:
		std::cout << "Unknown Segment " << (int)segment.marker[1] << ":" << std::endl;
		LogSegment(segment);
		throw RuntimeException("Unsupported segment " + std::to_string((int)segment.marker[1]) + "!");
	}
}

void JpegImage::ProcessApp0(const Segment& segment)
{
    // Only JFIF supported
    std::string jfif = "JFIF";
    std::string type = std::string((char*)&segment.data[0]);
    if (type != jfif)
		{
        throw RuntimeException("Unsupported APP0 format:" + type + "!");
    }

    // tbd jfif version check?
    std::cout << "JFIF version:  " << std::dec << (int)segment.data[5] << "." << (int)segment.data[6] << std::endl;
    // tbd checks?
    std::cout << "Density units: " << std::dec << (int)segment.data[7] << std::endl;
    std::cout << "Xdensity:      " << std::hex << (int)segment.data[8] << " " << (int)segment.data[9] << std::endl;
    std::cout << "Ydensity:      " << std::hex << (int)segment.data[10] << " " << (int)segment.data[11] << std::endl;

    // bytes at positions 12 and above are thumbnail data - not applicable for use-case
		}

void JpegImage::ProcessDqt(const  Segment& segment)
{
	// Supports only 8bit mode (1st half (msb) of 1st byte determines 8 (0) or 16 (!0) bit mode)
	if (segment.data[0] > 0x0f)
	{
		throw RuntimeException("Unsupported quantization table type!");
	}

	// TODO change handling of the type
	std::string dqtType = "Invalid";
	if ((segment.data[0] & 0x0f) == 0)
	{
		dqtType = "Luminance";
}
	else if ((segment.data[0] & 0x0f) == 1)
	{
		dqtType = "Chrominance";
	}
	else
	{
		throw std::runtime_error("Unsupported quantization table type (dqt):" + std::to_string((int)segment.data[0]& 0x0f));
	}

	// TODO process quantization tables - store in instance for later use
	// TODO Zig zag ?
	std::cout << "DQT: " << dqtType << std::endl;
	for (uint8_t a = 0; a < 8; a++)
	{
		for (uint8_t b = 0; b < 8; b++)
		{
			std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)segment.data[zigZag[a][b] + 1] << " ";
		}
		std::cout << std::endl;
	}
}

void JpegImage::ProcessSof(const  Segment& segment)
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
	// y:   6, 7, 8
	// Cb:  9,10,11
	// Cr: 12,13,14

	std::cout << "Start Of Frame:" << std::endl;
	std::cout << "Image Width  " << std::dec << (int)this->width << std::endl;
	std::cout << "Image Height " << std::dec << (int)this->height << std::endl;
	std::cout << "Component subsampling (#components " << (int)segment.data[5] << ")" << std::endl;
	for (uint8_t i = 0; i < segment.data[5]; i++)
	{
		std::string component = "??";
		if (segment.data[i * 3 + 6] == 1)
			component = "Y ";
		if (segment.data[i * 3 + 6] == 2)
			component = "Cb";
		if (segment.data[i * 3 + 6] == 3)
			component = "Cr";
		std::cout << component << ": "  << std::setw(2) << std::setfill('0') << std::hex << (int)segment.data[i * 3 + 7]
			<< " " << std::setw(2) << std::setfill('0') << std::hex << (int)segment.data[i * 3 + 8] << std::endl;
	}

	// TODO: progresive dct
	if (segment.marker[1] == 0xc2)
	{
		throw std::runtime_error("Progressive DCT not yet supported!");
	}

//                Precision Height  Width   #comp Y           Cb          Cr
//	ff c0, 15(15): 08        00 64   00 64   03    01 22 00    02 11 01    03 11 01
//  ff c0, 15(15): 08        00 0a   00 0a   03    01 22 00    02 11 01    03 11 01
//  ff c2, 15(15): 08        04 38   04 38   03    01 22 00    02 11 01    03 11 01
//  ff c2, 15(15): 08        04 a9   04 74   03    01 22 00    02 11 01    03 11 01
}

void JpegImage::ProcessSos(const Segment& segment)
{
	std::cout << "Sos:" << std::endl;
	LogSegment(segment);

	auto index = 0;
	std::cout << "Component count: " << std::dec << (int)segment.data[index++] << std::endl;
	for (uint8_t i = 0; i < segment.data[0]; i++)
	{
		std::string component = "??";
		if (segment.data[index] == 1)
			component = "Y ";
		if (segment.data[index] == 2)
			component = "Cb";
		if (segment.data[index] == 3)
			component = "Cr";
		index++; // increment index to get value
		std::cout << component << ": " << std::setw(2) << std::setfill('0') << std::hex << (int)segment.data[index] << std::endl; // TODO change to mark table
		index++; // increment for next loop
	}

	// skip next 3 bytes
	index += 3;

	// TODO ...;
	for (uint8_t i = index; i < segment.data.size(); i++)
	{
/*		if (segment.data[i] == 0xff)
		{
			std::cout << "FF ";
			continue;
		}*/

		for (int j = 0; j < 8; j++)
		{
			std::cout << ((segment.data[i] & 1 << j) ? "1" : "0");
		}
		std::cout << " ";
	}
}

void JpegImage::ProcessDht(const Segment& segment)
{
	std::cout << "Dht:" << std::endl;

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
	huffmanTables[segment.data[0]] = codes;

	/*
	// type of HT, 0 = DC table, 1 = AC table
	auto htType = segment.data[0] >> 4;

	// DC table
	if (htType == 0)
	{
		this->dhtDCTables[htNumber] = codes;
	}

	// AC table
	if (htType == 0)
	{
		this->dhtACTables[htNumber] = codes;
	}*/
	}
}
