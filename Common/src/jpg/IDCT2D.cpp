#include "jpg/IDCT2D.hpp"
#include <cmath>

static constexpr double PI = 3.14159265358979323846264338;

std::vector<double> IDCT2D::InverseDCT(const int32_t* inputBlock)
{
	std::vector<double> result(64);
	
	constexpr uint8_t M = 8;
	constexpr uint8_t N = 8;
	static const auto f = 2.0 / std::sqrt(static_cast<double>(M * N)); // 2 / (sqrt(M + N))

	// Calculate IDCT for 8x8 block
	for (uint8_t y = 0; y < 8; y++)
	{
		for (uint8_t x = 0; x < 8; x++)
		{
			// Perform IDCT calculation for element x,y
			double value = 0.0;
			
			for (uint8_t m = 0; m < M; m++) // y (row)
			{
				for (uint8_t n = 0; n < N; n++) // x (column)
				{
					const auto cm = m == 0 ? 1.0 / std::sqrt(2.0) : 1.0;
					const auto cn = n == 0 ? 1.0 / std::sqrt(2.0) : 1.0;
					value += cm * cn * inputBlock[m * 8 + n] *
							 std::cos(((2 * y + 1) * m * PI) / (2 * M)) *
							 std::cos(((2 * x + 1) * n * PI) / (2 * N));
				}
			}

			result[y * 8 + x] = f * value;
		}
	}
	return result;
}
