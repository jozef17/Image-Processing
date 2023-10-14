#pragma once

#ifndef JPEG_IMAGE_HPP__
#define JPEG_IMAGE_HPP__

#include <map>
#include <vector>
#include <string>

#include "Image.hpp"
#include "HuffmanCode.hpp"

struct Segment;

class JpegImage : public Image
{
public:
	JpegImage(const std::string& filename);

private:
	void LoadImage(const std::string& filename);

	void ProcessSegment(const Segment& segment);
	
	/// <summary>
	/// Process application specific metadata
	/// </summary>
	/// <param name="segment">Segment data</param>
	void ProcessAppn(const Segment& segment);

	/// <summary>
	/// Process Quantization table/s
	/// </summary>
	/// <param name="segment">Segment data</param>
	void ProcessDqt(const Segment& segment);

	/// <summary>
	/// Process metadata aka "start of frame"
	/// baseline and progresive dct
	/// </summary>
	/// <param name="segment">Segment data</param>
	void ProcessSf(const Segment& segment);

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

	/// Huffmann tables
	std::map<uint8_t, std::vector<HuffmanCode>> dhtDCTables;
	std::map<uint8_t, std::vector<HuffmanCode>> dhtACTables;

};

#endif /* JPEG_IMAGE_HPP__ */