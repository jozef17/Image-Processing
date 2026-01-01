#include "jpg/JpegLoader.hpp"
#include "jpg/JpegUtils.hpp"
#include "jpg/EntropyDecoder.hpp"
#include "jpg/JpegException.hpp"
#include "BitStream.hpp"

#include <fstream>
#include <sstream>
#include <cstdint>
#include <algorithm>
#include <iomanip>

constexpr uint8_t dqtTypeLuminance = 0;
constexpr uint8_t dqtTypeChrominance = 1;

enum class ColorComponent : uint8_t
{
	Y  = 1,
	Cb = 2,
	Cr = 3
};

struct Segment
{
	uint8_t marker[2]         = {};
	uint16_t size             = 0;
	std::vector<uint8_t> data = {};
};

JpegLoader::JpegLoader(const std::string& filename) : filename(filename)
{}

bool JpegLoader::IsJpegImage(const uint8_t* header, uint32_t size)
{
	// SOI marker  (ff d8) and start of next marker (ff)
	constexpr uint8_t jpegHeader[3] = { 0xff, 0xd8, 0xff };
	if (size < 3)
	{
		throw JpegException("Not enough data to asses the file (jpg)");
	}
	return std::memcmp(header, jpegHeader, 3) == 0;
}

std::unique_ptr<Image> JpegLoader::LoadJpegImage()
{
	LoadImage(this->filename);
	return std::move(this->image);
}

void JpegLoader::LoadImage(const std::string& filename)
{
	std::ifstream file(filename, std::ios::binary);
	if (!file.good())
	{
		throw JpegException("Failed to open file " + filename + ": " + strerror(errno));
	}

	uint8_t buffer[128];

	// Process start of image
	file.read((char*)&buffer, 2);
	if (buffer[0] != 0xff && buffer[1] != 0xd8)
	{
		throw JpegException("Not jpeg image!");
	}

	// Load segments
	Segment segment = {};
	while (file)
	{
		// Load segment marker
		file.read((char*)&segment.marker, 2);
		if (segment.marker[0] != 0xff)
		{
			std::stringstream ss;
			ss << "Was expecting start of segment marker, instead got " << std::setw(2) << std::setfill('0') << std::hex << (int)segment.marker[0] << "!";
			throw JpegException(ss.str());
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
			auto toRead = std::min(size, static_cast<uint16_t>(sizeof(buffer)));
			size -= toRead;

			file.read((char*)&buffer, toRead);
			segment.data.insert(segment.data.end(), buffer, buffer + toRead);
		} while (size > 0);

		ProcessSegment(segment);
		segment.data.clear();

		// Load compressed data - after SoS marker
		if (segment.marker[1] == 0xda)
		{
			std::vector<uint8_t> compressedData;
			uint8_t current;

			while (file.good())
			{
				file.read((char*)&current, 1);
				if (current != 0xff)
				{
					compressedData.push_back(current);
					continue;
				}

				// Get next byte after 0xff
				file.read((char*)&current, 1);
				if (current == 0x00)
				{
					// 0xff escaped - add only 0xff
					compressedData.push_back(0xff);
					continue;
				}

				// Marker detected
				try 
				{
					DecodeStream(compressedData);
					compressedData.clear();
				}
				catch (EndOfStreamException&)
				{
					// Restart marker detected - do nothing
				}

				// End if marker is not restart
				if (current < 0xd0 || current > 0xd7)
				{
					// End of stream, move back in file
					file.seekg(std::ios::cur - 2);
					break;
				}
			}
		} // bitstream loading and processing
	}// while
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
		throw JpegException("Progressive DCT not supported!");
	default:
		throw JpegException("Unsupported segment " + std::to_string((int)segment.marker[1]) + "!");
	}
}

// Cobtains quantization tables
// Precission (1/2 byte), Table ID (1/2 byte), 64 bytes of quantization data arranged in zig-zag order
void JpegLoader::ProcessDqt(const  Segment& segment)
{
	// Supports only 8bit mode (1st half (msb) of 1st byte determines 8 (0) or 16 (!0) bit mode)
	if (segment.data[0] >= 0x80)
	{
		throw JpegException("Unsupported quantization table type!");
	}

	// Only 8bit quantization tables supported
	if (segment.size != 65)
	{
		throw JpegException("Only one quantization table in DQT supported!");
	}

	uint8_t dqtType = segment.data[0] & 0x0f;
	if(dqtType > dqtTypeChrominance)
	{
		throw JpegException("Unsupported quantization table type (dqt):" + std::to_string((int)segment.data[0] & 0x0f));
	}

	std::vector<uint8_t> quantTable(64);
	std::copy(segment.data.begin() + 1, segment.data.begin() + 65, quantTable.begin());
	this->quantizationTables[dqtType] = quantTable;
}

// Start of frame - image metadata
// Precision (1 byte), Height (2 bytes), Width (2 bytes), Number of components (1 byte), Component info (3 bytes per component - ID, Sampling (1/2 width 1/2 height), Quantization table ID)
void JpegLoader::ProcessSof(const  Segment& segment)
{
	if (segment.data[0] != 8)
	{
		throw JpegException("Unsupported precision: " + std::to_string((int)segment.data[0]) + "!");
	}

	// YCbCr only
	if (segment.data[5] != 3)
	{
		throw JpegException("Unsupported component count: " + std::to_string((int)segment.data[5]) + "!");
	}

	auto height = ((int)segment.data[1] << 8 | (int)segment.data[2]);
	auto width  = ((int)segment.data[3] << 8 | (int)segment.data[4]);
	this->image = std::make_unique<Image>(width, height);

	for (uint8_t i = 0; i < segment.data[5]; i++)
	{
		SofComponentInfo info = {};
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
	}
}

// Start of scan - mapping between components and huffman tables
// Number of components (1 byte), ID and huffman table selection (2 bytes per component), other (3bytes)
void JpegLoader::ProcessSos(const Segment& segment)
{
	// Process header
	auto index = 1;
	for (uint8_t i = 0; i < segment.data[0]; i++)
	{
		uint8_t component = segment.data[index];
		index++; // increment index to get value
		componentHuffmanTables[component] = std::make_tuple(
			segment.data[index] >> 4,  // DC table
			segment.data[index] & 0x0f // AC table
		);
		index++; // increment for next loop
	}
}

// Deffine huffman tables
// Table class - DC/AC (1/2 byte), Table ID (1/2 byte), 16 bytes of code lengths, list of codes
void JpegLoader::ProcessDht(const Segment& segment)
{
	// last three bits has to be 0
	if (segment.data[0] >> 5)
	{
		throw JpegException("Invalid DHT data");
	}

	// htNumber = first half of byte
	auto htNumber = segment.data[0] & 0x0f;
	if (htNumber > 3)
	{
		throw JpegException("Invalid htNumber " + std::to_string(htNumber));
	}

	// list of code lengths and codes
	std::vector<HuffmanCode> codes;
	uint32_t nextIndex = 17; // index of next code
	// For all huffman code lengths (1 - 16)
	for (uint8_t i = 1; i <= 16; i++)
	{
		// Add all codes of the length i to the list
		for (uint8_t j = 0; j < segment.data[i]; j++)
		{
			// value, code (0 - to be assigned), length
			HuffmanCode entry = { segment.data[nextIndex++], 0, i };
			codes.push_back(entry);
		}
	}

	// type of HT, 0 = DC table, 1 = AC table
	auto htType = segment.data[0] >> 4;

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
}

void JpegLoader::DecodeStream(std::vector<uint8_t> &compressedData)
{
	// Create bitStream
	BitStream bitStream(BitStream::Mode::MSB);
	const auto length = compressedData.size();
	std::unique_ptr<uint8_t[]> data(new uint8_t[length]);
	std::memcpy(data.get(), compressedData.data(), length);

	bitStream.Append(std::move(data), static_cast<uint32_t>(length));

	// Initialize DC coefficients for each component
	std::map<uint8_t, int32_t> dcCoeficients; // component ID, last DC coefficient
	for (const auto& comp : this->components)
	{
		dcCoeficients[comp.componentId] = 0;
	}

	EntropyDecoder entropyDecoder(bitStream);
	while (true)
	{
		std::map<ColorComponent, std::vector<std::vector<double>>> mcuBlocks; // component, list of blocks

		for (const auto& component : this->components)
		{
			// Decode appropriate number of blocks for given component, based on scaling factors (chroma upsamling)
			for (uint8_t scale = 0; scale < component.sampFactorH * component.sampFactorV; scale++)
			{
				// Decode block
				const auto [dcTableId, acTableId] = this->componentHuffmanTables[component.componentId];
				auto block = entropyDecoder.DecodeBlock(this->dcTables[dcTableId], this->acTables[dcTableId]);

				// Update DC coefficient
				dcCoeficients[component.componentId] += block[0];
				block[0] = dcCoeficients[component.componentId];

				// Get quantization table for component
				auto quantTableType = static_cast<ColorComponent>(component.componentId) == ColorComponent::Y ? dqtTypeLuminance : dqtTypeChrominance;
				const auto& quantTable = this->quantizationTables[static_cast<uint8_t>(quantTableType)];

				// Dequantization - Elementwise multiplication of block by corresponding quantization table
				for (int index = 0; index < 64; index++)
				{
					block[index] = block[index] * static_cast<int32_t>(quantTable[index]);
				}

				// zig-zag reordering
				auto reorderedBlock = JpegUtils::ZigZagReorder(block);

				// Apply 2D inverse discrete cosine transform
				mcuBlocks[static_cast<ColorComponent>(component.componentId)].push_back(JpegUtils::InverseDCT(reorderedBlock));
			} // for scaling factor
		} // for components

		std::vector<YCbCrPixel> mcuPixels(8 * this->maxSampFactorH * 8 * this->maxSampFactorV); // pixels for current MCU

		// All decoded blocks for current MCU
		for (const auto& component : this->components)
		{
			auto colorComponentData = PrepareComponent(mcuBlocks[static_cast<ColorComponent>(component.componentId)]);
			// Merge components into pixels
			for (int i = 0; i < colorComponentData.size(); i++) // y position in block
			{
				switch (static_cast<ColorComponent>(component.componentId))
				{
				case ColorComponent::Y:
					mcuPixels[i].y = colorComponentData[i];
					break;
				case ColorComponent::Cb:
					mcuPixels[i].Cb = colorComponentData[i];
					break;
				case ColorComponent::Cr:
					mcuPixels[i].Cr = colorComponentData[i];
					break;
				}
			}
		} // Color component
		mcuBlocks.clear(); // No longer needed - clean up

		// Move to image buffer
		for (int y = 0; y < 8 * this->maxSampFactorH && this->mcuStartY + y < this->image->GetHeight(); y++)
		{
			for (int x = 0; x < 8 * this->maxSampFactorV && this->mcuStartX + x < this->image->GetWidth(); x++)
			{
				this->image->SetPixel(this->mcuStartX + x, this->mcuStartY + y, Pixel{ mcuPixels[y * 8 * this->maxSampFactorV + x] });
			}
		}

		// From top left to right, then down
		// Move to next MCU in MCU row
		this->mcuStartX += 8 * this->maxSampFactorV;
		// End of row, move to next MCU row
		if (this->mcuStartX >= this->image->GetWidth())
		{
			this->mcuStartX = 0;
			this->mcuStartY += 8 * this->maxSampFactorH;
			if (this->mcuStartY >= this->image->GetHeight())
			{
				break; // End of image
			}
		}
	}
}

std::vector<uint8_t> JpegLoader::PrepareComponent(const std::vector<std::vector<double>>& component)
{
	const uint8_t upsampling = this->maxSampFactorH * this->maxSampFactorV;
	std::vector<uint8_t> result(upsampling * 64);

	// 4:4:4 upsampling (no upsampling)
	if (upsampling == 1)
	{
		for (int i = 0; i < 64; i++)
		{
			auto value = std::round(component[0][i] + 128);
			value = value < 0 ? 0 : value > 255 ? 255 : value;
			result[i] = static_cast<uint8_t>(value);
		}
	}
	// 4:2:0 upsampling
	else if (upsampling == 4)
	{
		// Downsampled component
		if (component.size() == 1)
		{
			for (int y = 0; y < 16; y++) // y position in block
			{
				for (int x = 0; x < 16; x++) // x position in block
				{
					auto value = std::round(component[0][(y / 2) * 8 + (x / 2)] + 128);
					value = value < 0 ? 0 : value > 255 ? 255 : value;
					result[y * 16 + x] = static_cast<uint8_t>(value);
				}
			}
		} // 1
		else if (component.size() == 4)
		{
			uint8_t compID = 0;
			for (uint8_t a = 0; a < 2; a++)
			{
				for (uint8_t b = 0; b < 2; b++)
				{
					// Loop over block
					for (int y = 0; y < 8; y++) // y position in block
					{
						for (int x = 0; x < 8; x++) // x position in block
						{
							auto value = std::round(component[compID][y * 8 + x] + 128);
							value = value < 0 ? 0 : value > 255 ? 255 : value;
							result[(y + a * 8) * 16 + x + b * 8] = static_cast<uint8_t>(value);
						} // x
					} // y

					compID++;
				} // b				
			} // a
		} // 4
	}
	else
	{
		throw JpegException("Only 4:4:4 and 4:2:0 upsampling is supported!");
	}

	return result;
}
