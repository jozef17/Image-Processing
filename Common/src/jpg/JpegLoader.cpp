#include "jpg/JpegLoader.hpp"
#include "Exception.hpp"
#include "BitStream.hpp"

#include <fstream>
#include <sstream>
#include <cstdint>
#include <iomanip>

// SIMD
#if defined(_MSC_VER) && defined(_M_X64)
#include <immintrin.h>
#endif

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

JpegLoader::JpegLoader(const std::string& filename)
{
	LoadImage(filename);

	// Clear temporary data
	this->acTables.clear();
	this->dcTables.clear();
	this->components.clear();
}

bool JpegLoader::IsJpegImage(const uint8_t* header, uint32_t size)
{
	if (size < 4)
	{
		throw RuntimeException("Not enough data to asses the file (jpg)");
	}
	// TODO check jpeg header
	return false;
}

void JpegLoader::LoadImage(const std::string& filename)
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

		ProcessSegment(segment);

		// Load compressed data - after SoS marker
		if (segment.marker[1] == 0xda)
		{
			std::vector<uint8_t> compressedData;
			uint8_t current;

			while (file.good())
			{
				file >> current;
				if (current != 0xff)
				{
					compressedData.push_back(current);
					continue;
				}

				file >> current;
				if (current == 0x00)
				{
					// 0xff escaped - add only 0xff
					compressedData.push_back(0xff);
					continue;
				}

				// Marker detected
				EntropyDecode(compressedData);
				compressedData.clear();

				if (current >= 0xd0 && current <= 0xd7) // TODO: potentialy not needed to be deleted
				{
					std::cout << "SOS Restart interval detected: " << std::dec << (int)(current) << std::endl;
					continue;
				}
				else
				{
					// End of stream, move back in file
					file.seekg(std::ios::cur - 2);
					break;
				}
			}
		}

		segment.data.clear();
	}
}

void JpegLoader::ProcessSegment(const Segment& segment)
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
void JpegLoader::ProcessDqt(const  Segment& segment)
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

	std::vector<uint16_t> quantTable(64);
	// todo zig-zag reordering ?
	std::copy(segment.data.begin() + 1, segment.data.begin() + 65, quantTable.begin());
	this->quantizationTables[static_cast<uint8_t>(dqtType)] = quantTable;

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
// Precision (1 byte), Height (2 bytes), Width (2 bytes), Number of components (1 byte), Component info (3 bytes per component - ID, Sampling (1/2 width 1/2 height), Quantization table ID)
void JpegLoader::ProcessSof(const  Segment& segment)
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
void JpegLoader::ProcessSos(const Segment& segment)
{
	std::cout << "Sos:" << std::endl;
	LogSegment(segment);

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
}

// Deffine huffman tables
// Table class - DC/AC (1/2 byte), Table ID (1/2 byte), 16 bytes of code lengths, list of codes
void JpegLoader::ProcessDht(const Segment& segment)
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

void JpegLoader::EntropyDecode(std::vector<uint8_t> &compressedData)
{
	// Create bitStream
	BitStream bitStream(BitStream::Mode::MSB);
	const auto length = compressedData.size();
	std::unique_ptr<uint8_t[]> data(new uint8_t[length]);
	std::memcpy(data.get(), compressedData.data(), length);
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

	// For each MCU (Minimum Coded Unit)
	const auto mcuWidth = 8 * this->maxSampFactorH;
	const auto mcuHeight = 8 * this->maxSampFactorV;
	// const auto mcu = mcuHeight * mcuWidth; // Is this needed???

	// Initialize DC coefficients for each component
	std::map<uint8_t, int> dcCoeficients; // component ID, last DC coefficient
	for (const auto& comp : this->components)
	{
		dcCoeficients[comp.componentId] = 0;
	}

	// TODO handle case where image width/height is not multiple of MCU size
	// TODO need for temporary image buffer ?
	// TODO calculate number of MCUs in image and loop over them
	for (int x = 0; x < 1; x++)
	{
		for (const auto& component : this->components)
		{
			for (int y = 0; y < component.sampFactorH * component.sampFactorV; y++)
			{
				// Decode block
				auto block = DecodeBlock(bitStream, component);
				// Update DC coefficient
				dcCoeficients[component.componentId] += block[0];
				block[0] = dcCoeficients[component.componentId];

				// Get quantization table for component
				auto quantTableType = static_cast<ColorComponent>(component.componentId) == ColorComponent::Y ? DqtType::Luminance : DqtType::Chrominance;
				// TODO add check for existence of quantization table
				const auto& quantTable = this->quantizationTables[static_cast<uint8_t>(quantTableType)];

				// Dequantization - Elementwise multiplication of block by corresponding quantization table
#if defined(_MSC_VER) && defined(_M_X64) // SIMD version using AVX2 (Intel x64)
				for (int i = 0; i < 64; i += 32)
				{
					__m256i simdBlock = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&block[i]));
					__m256i simdQuant = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&quantTable[i]));
					__m256i simdMultipy = _mm256_mullo_epi16(simdBlock, simdQuant);
					_mm256_storeu_si256(reinterpret_cast<__m256i*>(&block[i]), simdMultipy);
				}
#else
				for (int i = 0; i < 64; i++)
				{
					block[i] = block[i] * quantTable[i];
				}
#endif

for (auto x : block)
std::cout << std::dec << x << " ";
std::cout << std::endl;
			}
		}
	}

	//	Sos:
	//	ff da, 10(00 c) : 03 01 00 02 11 03 11   00 3f 00   fd fc a2 8a 28 03
	// component ID, (DC table, AC table)
	//		Y  DC : 0, AC : 0
	//		Cb DC : 1, AC : 1
	//		Cr DC : 1, AC : 1
	// 11111101 11111100 10100010 10001010 00101000 00000011 ok
}

std::vector<int16_t> JpegLoader::DecodeBlock(BitStream& bitStream, const SofComponentInfo& component)
{
	std::map<uint8_t, int> dcCoeficients; // TODO move elsewhere!!!

	// TODO loop for each block in MCU according to sampling factors
	std::cout << "Component ID: " << std::dec << (int)component.componentId << std::endl;

	// Get Huffman tables for component
	const auto [dcTableId, acTableId] = this->componentHuffmanTables[component.componentId];
	std::cout << std::dec << " DC Table: " << (int)dcTableId << ", AC Table: " << (int)acTableId << std::endl;

	const auto& dcTable = this->dcTables[dcTableId];
	const auto& acTable = this->acTables[acTableId];

	// Decode DC coefficient
	auto dcLength = Decode(bitStream, dcTable);
	std::cout << "Decoded DC Value (length): " << std::dec << dcLength << std::endl;

	int dcValue = Read(bitStream, dcLength);
	std::cout << "Additional bits: " << std::dec << dcValue << std::endl;

	// Update DC coefficient
	dcCoeficients[component.componentId] += dcValue;
	std::cout << "Component: " << std::dec << (int)component.componentId << ", DC Coefficient: " << dcCoeficients[component.componentId] << std::endl;

	std::vector<int16_t> block(64); // Initialize all coefficients to zero
	block[0] = dcValue;
	// Decode AC coefficients
	for (int i = 1; i < 64; i++)
	{
		auto acValue = Decode(bitStream, acTable);
		std::cout << "Decoded AC Value: " << std::dec << acValue << std::endl;
		// Valie 0 represents End Of Block (EOB) - all remaining coefficients are zero
		if (acValue == 0)
		{
			break;
		}

		throw RuntimeException("Not implemented");
		// TODO process acValue
		// TODO if 0xF0 -> 16 zeroes
		// TODO else first 4 bits = number of leading zeroes, last 4 bits = length of additional bits
		break;
	}

	return block;
}

uint16_t JpegLoader::Decode(BitStream &bitStream, const std::vector<HuffmanCode>& huffmanTable) const
{
	uint16_t code = 0;

	// Read initial bits according to minimum code length
	for (int i = 0; i < huffmanTable[0].length - 1; i++)
	{
		uint16_t bit = bitStream.GetNext();
std::cout << "Next bit " << std::dec << (int)bit << std::endl;;
		code = code | bit << i;
	}

	// loop over code lengths from minimum to maximum
	for (int i = huffmanTable[0].length - 1; i <= huffmanTable[huffmanTable.size() - 1].length; i++)
	{
		uint16_t bit = bitStream.GetNext();
		code = code | bit << i;
//std::cout << "Next bit " << std::dec << (int)bit << std::endl;;
//std::cout << std::dec << "i: " << i << " code " << (int)code << ": ";
//for (int j = 0; j < 16; j++)
//{
//std::cout << ((code & 1 << j) ? "1" : "0");
//if (j == 7) std::cout << " ";
//}
//std::cout << std::endl;

		// Check if code matches any entry in table
		for(const auto& entry : huffmanTable)
		{
			if(entry.length <= i)
			{
				continue;
			} 
			else if (entry.length > i + 1)
			{
				break;
			}

if (entry.length != (i + 1))
std::cout << "????????????????????" << std::endl;
			if (entry.length == (i + 1) && entry.code == code)
			{
				return entry.value;
			}
		}
	}

	throw RuntimeException("Failed to decode DC coefficient!");
}

// Read additional bits
// First bit determines the sign
// if 1, number is positive and take as is
// if 0, number is negative and need to be subtracted by (2^value - 1)
int16_t JpegLoader::Read(BitStream& bitStream, uint8_t length) const
{
	int16_t value = 0;
	if (length > 0)
	{
		value = bitStream.GetNext();
		bool isNegative = value == 0; // First bit is a sign
		for (int i = 1; i < length; i++)
		{
			// TODO check if done right!!!
			value = (value << 1) | bitStream.GetNext();
		}

		if (isNegative)
		{
			value = value - ((1 << length) - 1);
		}
	}
	return value;
}