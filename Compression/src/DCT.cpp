#include "DCT.hpp"

#include <vector>
#include <thread>

#ifndef _WIN32
#define _USE_MATH_DEFINES
#endif
#include <cmath>
#ifdef _WIN32
#include <corecrt_math_defines.h>
#endif
#include <Pixel.hpp>

constexpr int threadCount = 8;

DCT::DCT(const FloatImage& image) : image(image)
{}

std::unique_ptr<FloatImage> DCT::dct()
{
	auto result = std::unique_ptr<FloatImage>(new FloatImage(this->image.GetWidth(), this->image.GetHeight()));

	auto applyDCTFunct = [&](int id) -> void
	{
		// Process (8x8) blocks

		for (unsigned int i = id * 8; i < this->image.GetHeight(); i += 8 * threadCount) // y
		{
			for (unsigned int j = 0; j < this->image.GetWidth(); j += 8) // x
			{
				// Process 8x8 blocks
				dctBlock(*result.get(), j, i);
			}
		}
	};

	// Run filter in parallel
	std::thread threads[threadCount - 1];
	for (int i = 0; i < threadCount - 1; i++)
	{
		threads[i] = std::thread(applyDCTFunct, i);
	}

	// Make use of main thread
	applyDCTFunct(threadCount - 1);

	// Wait for threads to finish
	for (int i = 0; i < threadCount - 1; i++)
	{
		threads[i].join();
	}

	return std::move(result);
}

std::unique_ptr<FloatImage> DCT::idct()
{
	auto result = std::unique_ptr<FloatImage>(new FloatImage(this->image.GetWidth(), this->image.GetHeight()));

	auto applyDCTFunct = [&](int id) -> void
	{
		// Process (8x8) blocks
		for (unsigned int i = id * 8; i < this->image.GetHeight(); i += 8 * threadCount) // y
		{
			for (unsigned int j = 0; j < this->image.GetWidth(); j += 8) // x
			{
				// Process 8x8 blocks
				idctBlock(*result.get(), j, i);
			}
		}
	};

	// Run filter in parallel
	std::thread threads[threadCount - 1];
	for (int i = 0; i < threadCount - 1; i++)
	{
		threads[i] = std::thread(applyDCTFunct, i);
	}

	// Make use of main thread
	applyDCTFunct(threadCount - 1);

	// Wait for threads to finish
	for (int i = 0; i < threadCount - 1; i++)
	{
		threads[i].join();
	}

	return std::move(result);
}

// Calculates DCT for 8 by 8 block
void DCT::dctBlock(FloatImage& result, int xOffset, int yOffset)
{
	double sum[3] = { 0,0,0 };

	auto M = (this->image.GetWidth() - xOffset) > 8 ? 8u : this->image.GetWidth() - xOffset;
	auto N = (this->image.GetHeight() - yOffset) > 8 ? 8u : this->image.GetHeight() - yOffset;

	auto f = 2 / std::sqrt((float)M * (float)N);

	// Loop at block
	for (auto m = 0u; m < M; m++)
	{
		for (auto n = 0u; n < N; n++)
		{
			// Clear sum
			std::memset(sum, 0, 3 * sizeof(double));

			// Calculate Cos Sum
			for (auto y = 0u; y < N; y++)
			{
				for (auto x = 0u; x < M; x++)
				{
					auto cos = std::cos(((2 * x + 1) * m * M_PI) / (2 * M)) * std::cos(((2 * y + 1) * n * M_PI) / (2 * N));

					auto data = this->image.Get(xOffset + x, yOffset + y);
					sum[0] += data.data[0] * cos;
					sum[1] += data.data[1] * cos;
					sum[2] += data.data[2] * cos;
				}
			}

			float cm = m == 0 ? 1.0f / std::sqrt(2.0f) : 1.0f;
			float cn = n == 0 ? 1.0f / std::sqrt(2.0f) : 1.0f;

			sum[0] *= f * cm * cn;
			sum[1] *= f * cm * cn; 
			sum[2] *= f * cm * cn; 

			result.Set(xOffset + m, yOffset + n, FloatData{ (float)sum[0],(float)sum[1],(float)sum[2]} );
		}
	}
}

// Calculates INverse DCT for 8 by 8 block
void DCT::idctBlock(FloatImage& result, int xOffset, int yOffset)
{
	double sum[3] = { 0,0,0 };

	auto M = (this->image.GetWidth() - xOffset) > 8 ? 8u : this->image.GetWidth() - xOffset;
	auto N = (this->image.GetHeight() - yOffset) > 8 ? 8u : this->image.GetHeight() - yOffset;

	auto f = 2.0 / std::sqrt((double)M * (double)N);

	// Loop at block
	for (auto y = 0u; y < N; y++)
	{
		for (auto x = 0u; x < M; x++)
		{
			// Clear sum
			std::memset(sum, 0, 3 * sizeof(double));

			// Calculate Sum
			for (auto n = 0u; n < N; n++)
			{
				for (auto m = 0u; m < M; m++)
				{
					auto cm = m == 0 ? 1.0 / std::sqrt(2) : 1.0;
					auto cn = n == 0 ? 1.0 / std::sqrt(2) : 1.0;
					auto cos = std::cos(((2 * x + 1) * m * M_PI) / (2 * M)) * std::cos(((2 * y + 1) * n * M_PI) / (2 * N));

					auto data = this->image.Get(xOffset + m, yOffset + n);
					sum[0] += cm * cn * data.data[0] * cos;
					sum[1] += cm * cn * data.data[1] * cos;
					sum[2] += cm * cn * data.data[2] * cos;
				}
			}

			// Multiply sum
			sum[0] *= f;
			sum[1] *= f;
			sum[2] *= f;

			result.Set(xOffset + x, yOffset + y, FloatData{ (float)sum[0],(float)sum[1],(float)sum[2]});
		}
	}
}
