#include "jpg/JpegUtils.hpp"
#include "Exception.hpp"

#include <cmath>

static constexpr double PI = 3.14159265358979323846264338;

std::vector<double> JpegUtils::InverseDCT(const std::vector<int32_t>& inputBlock)
{
	std::vector<double> result(64);
	if (inputBlock.size() != 64)
	{
		throw Exception("Input block size must be 8x8");
	}

	// Calculate IDCT for 8x8 block
	for (uint8_t y = 0; y < 8; y++)
	{
		for (uint8_t x = 0; x < 8; x++)
		{
			// Perform IDCT calculation for element x,y
			double sum = 0.0;
			
			for (uint8_t m = 0; m < 8; m++) // y (row)
			{
				for (uint8_t n = 0; n < 8; n++) // x (column)
				{
					const auto cm = m == 0 ? (1.0 / std::sqrt(2.0)) : 1.0;
					const auto cn = n == 0 ? (1.0 / std::sqrt(2.0)) : 1.0;
					sum += cm * cn * static_cast<double>(inputBlock[m * 8 + n]) *
							 std::cos(((2.0 * static_cast<double>(y) + 1) * static_cast<double>(m) * PI) / 16.0) *
							 std::cos(((2.0 * static_cast<double>(x) + 1) * static_cast<double>(n) * PI) / 16.0);
				}
			}

			result[y * 8 + x] = 0.25 * sum;
		}
	}
	return result;
}

std::vector<int32_t> JpegUtils::ZigZagReorder(const std::vector<int32_t>& zigZagBlock)
{
	if (zigZagBlock.size() != 64)
	{
		throw Exception("Input block size must be 8x8");
	}

	std::vector<int32_t> reorderedBlock(64);
	for (uint8_t i = 0; i < 64; i++)
	{
		reorderedBlock[zigZag[i]] = zigZagBlock[i];
	}
	return reorderedBlock;
}
