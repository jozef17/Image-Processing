#pragma once

#ifndef JPEG_UTILS_HPP__
#define JPEG_UTILS_HPP__

#include <cstdint>
#include <vector>

/// <summary>
/// Invverse 2D Discrete Cosine Transform
/// </summary>
class JpegUtils final
{
public:	
	JpegUtils() = delete;
	~JpegUtils() = delete;

	/// <summary>
	/// Apply Inverse 2D DCT on input 8x8 block
	/// </summary>
	/// <param name="inputBlock">Input block</param>
	static std::vector<double> InverseDCT(const std::vector<int32_t> &inputBlock);

	/// <summary>
	/// Reorder input 8x8 block from zig-zag order to normal order
	/// </summary>
	/// <param name="inputBlock">8x8 block in zig-zag order</param>
	/// <returns>reordered 8x8 block</returns>
	static std::vector<int32_t> ZigZagReorder(const std::vector<int32_t>& zigZagBlock);

private:
	/// <summary>
	/// Zig-zag oerdering table
	/// </summary>
	static constexpr uint8_t zigZag[64] = {
			 0,  1,  8, 16,  9,  2,  3, 10,
			17, 24, 32, 25, 18, 11,  4,  5,
			12, 19, 26, 33, 40, 48, 41, 34,
			27, 20, 13,  6,  7, 14, 21, 28,
			35, 42, 49, 56, 57, 50, 43, 36,
			29, 22, 15, 23, 30, 37, 44, 51,
			58, 59, 52, 45, 38, 31, 39, 46,
			53, 60, 61, 54, 47, 55, 62, 63
	};

};

#endif // JPEG_UTILS_HPP__