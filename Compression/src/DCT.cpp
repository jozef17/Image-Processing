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

constexpr int threadCount = 1;

DCT::DCT(Image& image) : image(std::move(image))
{}

std::unique_ptr<Image> DCT::dct()
{
	auto result = std::unique_ptr<Image>(new Image(this->image.GetWidth(), this->image.GetHeight()));

	// TODO lambda
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
	/*
	// Run filter in parallel
	std::thread threads[threadCount - 1];
	for (int i = 0; i < threadCount - 1; i++)
	{
		threads[i] = std::thread(applyDCTFunct, i);
	}*/

	// Make use of main thread
	applyDCTFunct(threadCount - 1);
	/*
	// Wait for threads to finish
	for (int i = 0; i < threadCount - 1; i++)
	{
		threads[i].join();
	}*/

	return std::move(result);
}

std::unique_ptr<Image> DCT::idct()
{
	auto result = std::unique_ptr<Image>(new Image(this->image.GetWidth(), this->image.GetHeight()));

	// TODO lambda
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
	/*
	// Run filter in parallel
	std::thread threads[threadCount - 1];
	for (int i = 0; i < threadCount - 1; i++)
	{
		threads[i] = std::thread(applyDCTFunct, i);
	}
	*/
	// Make use of main thread
	applyDCTFunct(threadCount - 1);
	/*
	// Wait for threads to finish
	for (int i = 0; i < threadCount - 1; i++)
	{
		threads[i].join();
	}*/

	return std::move(result);
}

// Calculates DCT for 8 by 8 block
void DCT::dctBlock(Image& result, int xOffset, int yOffset)
{
	double sum[3] = { 0,0,0 };

	int M = (this->image.GetWidth() - xOffset) > 8 ? 8 : this->image.GetWidth() - xOffset;
	int N = (this->image.GetHeight() - yOffset) > 8 ? 8 : this->image.GetHeight() - yOffset;

	double f = (double)2.0 / std::sqrt((double)M * (double)N);

	// Loop at block
	for (int m = 0; m < M; m++)
	{
		for (int n = 0; n < N; n++)
		{
			// Clear sum
			for (char c = 0; c < 3; c++)
				sum[c] = 0;

			// Calculate Cos Sum
			for (int x = 0; x < M; x++)
			{
				for (int y = 0; y < N; y++)
				{
					double cos = std::cos(((2 * x + 1) * m * M_PI) / (2 * M)) * std::cos(((2 * y + 1) * n * M_PI) / (2 * N));
					
					auto pixel = this->image.GetPixel(xOffset + x, yOffset + y).ToYCbCr();
					sum[0] += pixel.y  * cos;
					sum[1] += pixel.Cb * cos;
					sum[2] += pixel.Cr * cos;
				}
			}

			double cm = m == 0 ? 1.0 / std::sqrt(2.0) : 1.0;
			double cn = n == 0 ? 1.0 / std::sqrt(2.0) : 1.0;

			sum[0] *= f * cm * cn;
			sum[1] *= f * cm * cn;
			sum[2] *= f * cm * cn;

			YCbCrPixel pixel
				= { static_cast<uint8_t>(sum[0]), static_cast<uint8_t>(sum[1]), static_cast<uint8_t>(sum[2]) };
			result.SetPixel(xOffset + m, yOffset + n, Pixel{ pixel });
		}
	}
}

// Calculates INverse DCT for 8 by 8 block
void DCT::idctBlock(Image& result, int xOffset, int yOffset)
{
	double sum[3] = { 0,0,0 };

	int M = (this->image.GetWidth() - xOffset) > 8 ? 8 : this->image.GetWidth() - xOffset;
	int N = (this->image.GetHeight() - yOffset) > 8 ? 8 : this->image.GetHeight() - yOffset;

	double f = (double)2.0 / std::sqrt((double)M * (double)N);

	// Loop at block
	for (int y = 0; y < N; y++)
	{
		for (int x = 0; x < M; x++)
		{
			// Clear sum
			for (char c = 0; c < 3; c++)
				sum[c] = 0;

			// Calculate Sum
			for (int m = 0; m < M; m++)
			{
				for (int n = 0; n < N; n++)
				{
					double cm = m == 0 ? 1.0 / std::sqrt(2) : 1.0;
					double cn = n == 0 ? 1.0 / std::sqrt(2) : 1.0;
					double cos = std::cos(((2 * x + 1) * m * M_PI) / (2 * M)) * std::cos(((2 * y + 1) * n * M_PI) / (2 * N));

					auto pixel = this->image.GetPixel(xOffset + m, yOffset + n).ToYCbCr();
					sum[0] += cm * cn * pixel.y  * cos;
					sum[1] += cm * cn * pixel.Cb * cos;
					sum[2] += cm * cn * pixel.Cr * cos;
				}
			}

			// Multiply sum
			sum[0] *= f;
			sum[1] *= f;
			sum[2] *= f;

			YCbCrPixel pixel
				= { static_cast<uint8_t>(sum[0]) ,static_cast<uint8_t>(sum[1]) ,static_cast<uint8_t>(sum[2]) };
			result.SetPixel(xOffset + x, yOffset + y, Pixel{ pixel });
		}
	}
}
