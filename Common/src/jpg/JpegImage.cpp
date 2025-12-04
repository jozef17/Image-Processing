#include "jpg/JpegImage.hpp"
#include "Exception.hpp"
#include "BitStream.hpp"

#include <fstream>
#include <sstream>
#include <cstdint>
#include <iomanip>

// TMP ////////////////////////////////////////////////////
#include <iostream>
// TMP ////////////////////////////////////////////////////

// TODO:
// Check DC - 0 / AC = 1 table class IDs
// Reject more than one DC & AC table per component
// Must mach SoF components ???
// reject multiple SoS segments
// Check that huffman tablôes exists for all components in SoS
// Allow only 4:4:4 and 4:2:0 subsampling, reject others

enum class DqtType : uint8_t
{
	Luminance = 0,
	Chrominance = 1,
	Invalid
};

enum class ColorComponent : uint8_t
{
	Y = 1,
	Cb = 2,
	Cr = 3
};

int DecodeHuffman(BitStream& bitStream, const std::vector<HuffmanCode>& table) {
	uint16_t code = 0;
	for (uint8_t length = 1; length <= 16; ++length) {
		code = (code << 1) | bitStream.GetNext();
		for (const auto& entry : table) {
			if (entry.length == length && entry.code == code) {
				return entry.value;
			}
		}
	}
	throw std::runtime_error("Invalid Huffman code encountered");
}

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
	uint16_t size;
	std::vector<uint8_t> data;
};

void LogSegment(const Segment& segment)
{
	std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)segment.marker[0] << " " << (int)segment.marker[1] << ", " <<
		std::dec << segment.size << ", ";
	for (auto c : segment.data)
	{
		std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)c << " ";
	}
	std::cout << std::endl;
}

JpegImage::JpegImage(const std::string& filename)
{
	LoadImage(filename);

	// Clear temporary data
	this->acTables.clear();
	this->dcTables.clear();
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
		// Load segment marker
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

		// Load segment size
		file.read((char*)&buffer, 2);
		segment.size = ((int)buffer[0] << 8 | (int)buffer[1]) - 2;

		// Load segment data
		auto size = segment.size;
		do
		{
			auto toRead = size < sizeof(buffer) ? size : sizeof(buffer);
			size -= toRead;

			file.read((char*)&buffer, toRead);
			segment.data.insert(segment.data.end(), buffer, buffer + toRead);
		} while (size > 0);

		// Load compressed data - after SoS marker
		if (segment.marker[1] == 0xda)
		{
			uint8_t current;
			while (file.good())
			{
				file >> current;
				if (current == 0xff)
				{
					file >> current;

					if (current >= 0xd0 && current <= 0xd7)
					{
						// TODO handle restart interval
						std::cout << "SOS Restart interval detected: " << std::dec << (int)(current) << std::endl;
						continue;
					}
					else if (current == 0x00)
					{
						// 0xff escaped - add only 0xff
						segment.data.push_back(0xff);
					}
					else
					{
						// End of stream, move back in file
						file.seekg(std::ios::cur - 2);
						break;
					}
				}
				else
				{
					segment.data.push_back(current);
				}
			}
		}

		ProcessSegment(segment);
		segment.data.clear();
	}
}

void JpegImage::ProcessSegment(const Segment& segment)
{
	// Skip application specific segments & comment (0xfe)
	if (segment.marker[1] >= 0xe0 && segment.marker[1] <= 0xef || segment.marker[1] == 0xfe)
	{
		return;
	}

	switch (segment.marker[1])
	{
	case 0xdb: // Define Quantization Tables (DQT)
		ProcessDqt(segment);
		break;
	case 0xc0: // "Start of frame" (SOF) - baseline DCT
		ProcessSof(segment); // Image metadata
		break;
	case 0xda: // Start of stream (SoS)
		ProcessSos(segment);
		break;
	case 0xc4: // Define Huffman table (DHT)
		ProcessDht(segment);
		break;
	case 0xc2: // "Start of frame" (SOF) - progresive DCT
		throw RuntimeException("Progressive DCT not supported!");
	default:
		LogSegment(segment);
		throw RuntimeException("Unsupported segment " + std::to_string((int)segment.marker[1]) + "!");
	}
}

// Cobtains quantization tables
// Precission (1/2 byte), Table ID (1/2 byte), 64 bytes of quantization data arranged in zig-zag order
void JpegImage::ProcessDqt(const  Segment& segment)
{
	// Supports only 8bit mode (1st half (msb) of 1st byte determines 8 (0) or 16 (!0) bit mode)
	if (segment.data[0] > 0x0f)
	{
		throw RuntimeException("Unsupported quantization table type!");
	}

	// Todo check if needed to be reworked
	if (segment.size > 65)
	{
		throw RuntimeException("Only one quantization table in DQT supported!");
	}

	DqtType dqtType = DqtType::Invalid;
	if ((segment.data[0] & 0x0f) == 0)
	{
		dqtType = DqtType::Luminance;
	}
	else if ((segment.data[0] & 0x0f) == 1)
	{
		dqtType = DqtType::Chrominance;
	}
	else
	{
		throw std::runtime_error("Unsupported quantization table type (dqt):" + std::to_string((int)segment.data[0] & 0x0f));
	}

	std::vector<uint8_t> quantTable(64);
	// todo zig-zag reordering ?
	std::copy(segment.data.begin() + 1, segment.data.begin() + 65, quantTable.begin());
	this->quantizationTables[(uint8_t)dqtType] = quantTable;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	std::cout << "DQT: " << (dqtType == DqtType::Chrominance ? "Chrominance" : "Luminance") << std::endl;
	for (uint8_t a = 0; a < 8; a++)
	{
		for (uint8_t b = 0; b < 8; b++)
		{
			std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)(this->quantizationTables[(uint8_t)dqtType][zigZag[a][b]]) << " ";
		}
		std::cout << std::endl;
	}
	std::cout << "---------------------------------------------------------------------" << std::endl;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

// Start of frame - image metadata
// Precision (1 byte), Height (2 bytes), Width (2 bytes), Number of components (1 byte), Component info (3 bytes per component - ID, Sampling (1/2 ), Quantization table ID)
void JpegImage::ProcessSof(const  Segment& segment)
{
	if (segment.data[0] != 8)
	{
		throw RuntimeException("Unsupported precision: " + std::to_string((int)segment.data[0]) + "!");
	}

	// YCbCr only
	if (segment.data[5] != 3)
	{
		throw RuntimeException("Unsupported component count: " + std::to_string((int)segment.data[5]) + "!");
	}

	this->width = ((int)segment.data[1] << 8 | (int)segment.data[2]);
	this->height = ((int)segment.data[3] << 8 | (int)segment.data[4]);

	for (uint8_t i = 0; i < segment.data[5]; i++)
	{
		SofComponentInfo info = {};

//		ColorComponent component = (ColorComponent)segment.data[i * 3 + 6]; // tbd if needed
		info.componentId  = segment.data[i * 3 + 6];
		info.sampFactorH  = (segment.data[i * 3 + 7] & 0xf0) >> 4;
		info.sampFactorV  = (segment.data[i * 3 + 7] & 0x0f);
		info.quantTableId = segment.data[i * 3 + 8];

		this->components.push_back(info);

		// Update max sampling factors
		if (info.sampFactorH > this->maxSampFactorH)
		{
			this->maxSampFactorH = info.sampFactorH;
		}

		if (info.sampFactorV > this->maxSampFactorV)
		{
			this->maxSampFactorV = info.sampFactorV;
		}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		ColorComponent component = (ColorComponent)segment.data[i * 3 + 6];
		std::string componentName = "??";
		if (component == ColorComponent::Y)
			componentName = "Y ";
		else if (component == ColorComponent::Cb)
			componentName = "Cb";
		else if (component == ColorComponent::Cr)
			componentName = "Cr";

		std::cout << componentName << ": " << "Sampling factors H: " << std::dec << (int)info.sampFactorH
			<< ", V: " << std::dec << (int)info.sampFactorV << ", " << " Quantization table ID:"
			<< std::dec << (int)info.quantTableId << std::endl;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}

	//                Precision Height  Width   #comp Y                        Cb          Cr
	//												  ID  SampFactor  QTableID
	//	ff c0, 15(15): 08        00 64   00 64   03    01 22          00          02 11 01    03 11 01
	//  ff c0, 15(15): 08        00 0a   00 0a   03    01 22          00          02 11 01    03 11 01
	//  ff c2, 15(15): 08        04 38   04 38   03    01 22          00          02 11 01    03 11 01
	//  ff c2, 15(15): 08        04 a9   04 74   03    01 22          00          02 11 01    03 11 01
}

// Start of scan - mapping between components and huffman tables
// Number of components (1 byte), ID and huffman table selection (2 bytes per component), other (3bytes)
void JpegImage::ProcessSos(const Segment& segment)
{
	std::cout << "Sos:" << std::endl;
	LogSegment(segment);

	// component ID, (DC table, AC table)
	std::map <uint8_t, std::tuple<uint8_t, uint8_t>> componentHuffmanTables;

	// Process header
	auto index = 1;
	std::cout << "Component count: " << std::dec << (int)segment.data[0] << std::endl;
	for (uint8_t i = 0; i < segment.data[0]; i++)
	{
		uint8_t component = segment.data[index];

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		std::string componentStr = "??";
		if (segment.data[index] == 1)
			componentStr = "Y ";
		if (segment.data[index] == 2)
			componentStr = "Cb";
		if (segment.data[index] == 3)
			componentStr = "Cr";
		index++; // increment index to get value
		std::cout << componentStr << " DC: " << ((int)segment.data[index] >> 4) << ", AC: " << (int)(segment.data[index] & 0x0f) << std::endl;
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////

		componentHuffmanTables[component] = std::make_tuple(
			segment.data[index] >> 4, // DC table
			segment.data[index] & 0x0f // AC table
		);
		index++; // increment for next loop
	}

	// skip next 3 bytes
	index += 3;

	// Create bitStream
	BitStream bitStream(BitStream::Mode::MSB);
	const auto length = segment.data.size() - index;
	std::unique_ptr<uint8_t[]> data(new uint8_t[length]);
	std::memcpy(data.get(), &segment.data[index], length);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	for (uint8_t i = 0; i < length; i++)
	{
		for (int j = 7; j >= 0; j--)
			std::cout << ((data[i] & 1 << j) ? "1" : "0");
		std::cout << " ";
	}
	std::cout << std::endl;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	bitStream.Append(std::move(data), length);

// ?????????????????????????????????????????????????????????????????????????????????????????????????????????	
    std::vector<int16_t> block(64, 0);

    // 1. Decode DC coefficient
    auto dcTableId = std::get<0>(componentHuffmanTables[components.at(0).componentId]);
	const auto& dcTable = this->dcTables[dcTableId];

	// For each MCU (Minimum Coded Unit)
	const auto mcuWidth = 8 * this->maxSampFactorH;
	const auto mcuHeight = 8 * this->maxSampFactorV;
	const auto mcu = mcuHeight * mcuWidth;

	// Initialize DC coefficients for each component
	std::map<uint8_t, int> dcCoeficients; // component ID, last DC coefficient
	for (const auto& comp : components)
	{
		dcCoeficients[comp.componentId] = 0;
	}

	// TODO handle case where image width/height is not multiple of MCU size
	// TODO need for temporary image buffer ?
	for (int i = 0; i < mcu; i++)
	{
		for (const auto& comp : components)
		{
			std::cout << "Component ID: " << std::dec << (int)comp.componentId << std::endl;

			// Get Huffman tables for component
			auto dcTableId = std::get<0>(componentHuffmanTables[comp.componentId]);
			auto acTableId = std::get<1>(componentHuffmanTables[comp.componentId]);
			std::cout << std::dec << " DC Table: " << (int)dcTableId << ", AC Table: " << (int)acTableId << std::endl;

			auto dcTable = this->dcTables[dcTableId];
			auto acTable = this->acTables[acTableId];

			// Decode DC coefficient
			// TODO move to separate function / class
			int code = 0;
			int value = -1;

			// Read minimum code length - 1 bits
			std::cout << "Decoding: ";
			for (int i = 0; i < dcTable[0].length -1; i++)
			{
				auto bit = bitStream.GetNext();
				std::cout << std::dec << (int)bit;
				code = (code << 1) | bit;
			}

			// loop over code lengths from minimum to maximum
			for (int i = dcTable[0].length; i <= dcTable[dcTable.size() - 1].length && value == -1; i++)
			{
				auto bit = bitStream.GetNext();
				std::cout << std::dec << (int)bit;
				code = (code << 1) | bit;

				// Check if code matches any entry in table
				for (const auto& entry : dcTable)
				{
					if (entry.length == i && entry.code == code)
					{
						value = entry.value;
						break;
					}
					else if (entry.length > i)
					{
						break;
					}
				}
			}

			if (value == -1)
			{
				throw RuntimeException("Failed to decode DC coefficient!");
			}
			std::cout << "DC Value: " << std::dec << value << std::endl;

			// Read additional bits
			// First bit determines the sign
			// if 1, number is positive and take as is
			// if 0, number is negative and need to be subtracted by (2^value - 1)
			int additionalBits = bitStream.GetNext();
			bool isNegative = additionalBits == 0;
			for (int i = 1; i < value; i++)
			{
				additionalBits = (additionalBits << 1) | bitStream.GetNext();
			}
			if (isNegative)
			{
				additionalBits = additionalBits - ((1 << value) - 1);
			}
			std::cout << "Additional bits: " << std::dec << additionalBits << std::endl;

			// Update DC coefficient
			dcCoeficients[comp.componentId] += additionalBits;
			std::cout << "Component: " << std::dec << (int)comp.componentId << ", DC Coefficient: " << dcCoeficients[comp.componentId] << std::endl;


			break; // TODO remove
		}
		break; // TODO remove
	}

//	Sos:
//	ff da, 10(00 c) : 03 01 00 02 11 03 11   00 3f 00   fd fc a2 8a 28 03
// component ID, (DC table, AC table)
//		Y  DC : 0, AC : 0
//		Cb DC : 1, AC : 1
//		Cr DC : 1, AC : 1
// 10111111 00111111 01000101 01010001 00010100 11000000 wrong!
// 11111101 11111100 10100010 10001010 00101000 00000011 ok
}

// Deffine huffman tables
// Table class - DC/AC (1/2 byte), Table ID (1/2 byte), 16 bytes of code lengths, list of codes
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
		throw RuntimeException("Invalid htNumber " + std::to_string(htNumber));
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

	// type of HT, 0 = DC table, 1 = AC table
	auto htType = segment.data[0] >> 4;

	std::cout << (htType == 0 ? "DC" : "AC") << " table " << std::dec << (int)htNumber << std::endl;
	// Generate codes for code / lengths
	HuffmanCode::AsignCodes(codes);

	// DC table
	if (htType == 0) 
	{
		this->dcTables[htNumber] = codes;
	}

	// AC table
	if (htType == 1)
	{
		this->acTables[htNumber] = codes;
	}
	std::cout << std::endl;
}
