#pragma once

#ifndef JPEG_IMAGE_HPP__
#define JPEG_IMAGE_HPP__

#include <map>
#include <vector>
#include <string>

#include "Image.hpp"
#include "HuffmanCode.hpp"

struct Segment;
class BitStream;

struct SofComponentInfo
{
	uint8_t componentId;
	uint8_t sampFactorH;
	uint8_t sampFactorV;
	uint8_t quantTableId;
};

/// <summary>
/// Minimal jpeg image loader
/// - Baseline DCT only (no progresive)
/// - 8bit per channel only
/// - YCbCr color space only
/// </summary>
class JpegLoader : public Image
{
public:
	JpegLoader(const std::string& filename);

	static bool IsJpegImage(const uint8_t* header, uint32_t size);

	// TODO load image

private:
	void LoadImage(const std::string& filename);

	void ProcessSegment(const Segment& segment);
	
	/// <summary>
	/// Process Quantization tables
	/// </summary>
	/// <param name="segment">Segment data</param>
	void ProcessDqt(const Segment& segment);

	/// <summary>
	/// Process image metadata aka "start of frame"
	/// baseline and progresive dct
	/// </summary>
	/// <param name="segment">Segment data</param>
	void ProcessSof(const Segment& segment);

	/// <summary>
	/// Process start of scan
	/// </summary>
	/// <param name="segment">Segment data</param>
	void ProcessSos(const Segment& segment);

	/// <summary>
	/// Process huffman table
	/// </summary>
	/// <param name="segment">Segment data</param>
	void ProcessDht(const Segment& segment);

	void EntropyDecode(std::vector<uint8_t> &compressedData);

	/// <summary>
	/// Decode single value from bit stream using provided huffman table
	/// </summary>
	/// <param name="bitStream">Encoded bitstream</param>
	/// <param name="huffmanTable">Huffman Table to be used for decoding</param>
	/// <returns>Decoded value, an exception is thrown when decoding fails</returns>
	uint16_t Decode(BitStream& bitStream, const std::vector<HuffmanCode>& huffmanTable) const;

	int16_t Read(BitStream& bitStream, uint8_t size) const;

	/// Huffmann tables
	/// key: table ID, value: list of codes
	std::map<uint8_t, std::vector<HuffmanCode>> dcTables;
	std::map<uint8_t, std::vector<HuffmanCode>> acTables;

	/// Quantization tables
	std::map<uint8_t, std::vector<uint8_t>> quantizationTables;

	/// Subsampling (& quantization table ID)
	/// component ID, sampling factor H,V, quantization table ID
	std::vector<SofComponentInfo> components;
	uint8_t maxSampFactorH = 0;
	uint8_t maxSampFactorV = 0;

	/// component ID, (DC table, AC table)
	std::map <uint8_t, std::tuple<uint8_t, uint8_t>> componentHuffmanTables;

};

#endif /* JPEG_IMAGE_HPP__ */