#include "ImageCompressor.hpp"
#include "DCT.hpp"
#include "Pixel.hpp"

#include <cmath>

// Quantization tables
static constexpr uint8_t QL[8][8] = { { 16, 11, 10, 16,  24,  40,  51,  61 },
									  { 12, 12, 14, 19,  26,  58,  60,  55 },
									  { 14, 13, 16, 24,  40,  57,  69,  56 },
									  { 14, 17, 22, 29,  51,  87,  80,  62 },
									  { 18, 22, 37, 56,  68, 109, 103,  77 },
									  { 24, 35, 55, 64,  81, 104, 113,  92 },
									  { 49, 64, 78, 87, 103, 121, 120, 101 },
									  { 72, 92, 95, 98, 112, 100, 103,  99 }};

static constexpr uint8_t QC[8][8] = { { 17, 18, 24, 47, 99, 99, 99, 99 },
									  { 18, 21, 26, 66, 99, 99, 99, 99 },
									  { 24, 26, 56, 99, 99, 99, 99, 99 },
									  { 47, 66, 99, 99, 99, 99, 99, 99 },
									  { 99, 99, 99, 99, 99, 99, 99, 99 },
									  { 99, 99, 99, 99, 99, 99, 99, 99 },
									  { 99, 99, 99, 99, 99, 99, 99, 99 },
									  { 99, 99, 99, 99, 99, 99, 99, 99 }};

ImageCompressor::ImageCompressor(const FloatImage &image, float quality)
	: image(image), alpha(GetAlpha(quality))
{}

std::unique_ptr<FloatImage> ImageCompressor::Encode()
{
	std::unique_ptr<FloatImage> result
		= std::unique_ptr<FloatImage>(new FloatImage(this->image.GetWidth(), this->image.GetHeight()));

	DCT dct(this->image);
	auto compressed = dct.dct();

	// Process 8 by 8 blocks
	for (auto i = 0u; i < this->image.GetWidth(); i += 8)
	{
		for (auto j = 0u; j < this->image.GetHeight(); j += 8)
		{
			int M = (compressed->GetWidth()  - i) > 8 ? 8 : compressed->GetWidth()  - i;
			int N = (compressed->GetHeight() - j) > 8 ? 8 : compressed->GetHeight() - j;

			// Process Block
			for (auto x = 0u; x < M; x++)
			{
				for (auto y = 0u; y < N; y++)
				{
					auto data = compressed->Get(i + x, j + y);
					data.data[0] = std::round(data.data[0] / (this->alpha * QL[x][y]));
					data.data[1] = std::round(data.data[1] / (this->alpha * QC[x][y]));
					data.data[2] = std::round(data.data[2] / (this->alpha * QC[x][y]));

					result->Set(i + x, j + y, data);
				} // y
			} // x
		}// b
	}// i

	return std::move(result);
}

std::unique_ptr<FloatImage> ImageCompressor::Decode()
{
	FloatImage tmp(this->image.GetWidth(), this->image.GetHeight());

	for (unsigned int i = 0; i < this->image.GetWidth(); i += 8)
	{
		for (unsigned int j = 0; j < this->image.GetHeight(); j += 8)
		{
			int M = (this->image.GetWidth()  - i) > 8 ? 8 : this->image.GetWidth()  - i;
			int N = (this->image.GetHeight() - j) > 8 ? 8 : this->image.GetHeight() - j;

			// Process Block
			for (int x = 0; x < M; x++)
			{
				for (int y = 0; y < N; y++)
				{
					auto data = this->image.Get(i + x, j + y);;
					data.data[0] = data.data[0] * (this->alpha * QL[x][y]);
					data.data[1] = data.data[1] * (this->alpha * QC[x][y]);
					data.data[2] = data.data[2] * (this->alpha * QC[x][y]);

					tmp.Set(i + x, j + y, data);
				} // y
			} // x
		} // b
	} // i

	DCT dct(tmp);
	return dct.idct();
}

float ImageCompressor::GetAlpha(float quality)
{
	if (quality < 50.f)
	{
		return 50.f / (float)quality;
	}
	return 2.f - (float)quality / 50.f;
}
