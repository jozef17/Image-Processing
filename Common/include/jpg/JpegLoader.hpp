#pragma once

#ifndef JPEG_IMAGE_HPP__
#define JPEG_IMAGE_HPP__

#include <map>
#include <vector>
#include <string>
#include <memory>

#include "Image.hpp"
#include "HuffmanCode.hpp"

struct Segment;
class BitStream;

/// <summary>
/// Minimal jpeg image loader
/// - Baseline DCT only (no progresive)
/// - 8bit per channel only
/// - YCbCr color space only
/// </summary>
class JpegLoader final
{
public:
	JpegLoader(const std::string& filename);

	/// <summary>
	/// Checks if provided header belongs to jpeg image
	/// </summary>
	/// <param name="header">header data</param>
	/// <param name="size">length of data</param>
	/// <returns></returns>
	static bool IsJpegImage(const uint8_t* header, uint32_t size);

	/// <summary>
	/// Loads jpg image
	/// </summary>
	/// <returns>loaded images</returns>
	std::unique_ptr<Image> LoadJpegImage();

private:

	/// <summary>
	/// Color component information
	/// </summary>
	struct SofComponentInfo
	{
		uint8_t componentId;
		uint8_t sampFactorH;
		uint8_t sampFactorV;
		uint8_t quantTableId;
	};

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

	void DecodeStream(std::vector<uint8_t> &compressedData);

	std::vector<uint8_t> PrepareComponent(const std::vector<std::vector<double>>& component);

	/// Huffmann tables
	/// key: table ID, value: list of codes
	std::map<uint8_t, std::vector<HuffmanCode>> dcTables;
	std::map<uint8_t, std::vector<HuffmanCode>> acTables;

	/// Quantization tables
	/// key table type (chrominance / luminance), value: quantization data
	std::map<uint8_t, std::vector<uint8_t>> quantizationTables;

	/// Subsampling (& quantization table ID)
	/// component ID, sampling factor H,V, quantization table ID
	std::vector<SofComponentInfo> components;
	/// Maximum sampling factors
	uint8_t maxSampFactorH = 0;
	uint8_t maxSampFactorV = 0;

	/// Mapping between components and huffman tables
	/// component ID, (DC table, AC table)
	std::map <uint8_t, std::tuple<uint8_t, uint8_t>> componentHuffmanTables;

	/// Start of next MCU
	uint32_t mcuStartX = 0;
	uint32_t mcuStartY = 0;

	std::string filename;
	std::unique_ptr<Image> image;
};

#endif /* JPEG_IMAGE_HPP__ */