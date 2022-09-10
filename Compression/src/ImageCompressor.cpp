#include "ImageCompressor.hpp"
#include "DCT.hpp"
#include "Image.hpp"
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
									  { 72, 92, 95, 98, 112, 100, 103,  99 } };

static constexpr uint8_t QC[8][8] = { { 17, 18, 24, 47, 99, 99, 99, 99 },
									  { 18, 21, 26, 66, 99, 99, 99, 99 },
									  { 24, 26, 56, 99, 99, 99, 99, 99 },
									  { 47, 66, 99, 99, 99, 99, 99, 99 },
									  { 99, 99, 99, 99, 99, 99, 99, 99 },
									  { 99, 99, 99, 99, 99, 99, 99, 99 },
									  { 99, 99, 99, 99, 99, 99, 99, 99 },
									  { 99, 99, 99, 99, 99, 99, 99, 99 } };

ImageCompressor::ImageCompressor(std::unique_ptr<Image> image, float quality)
	: image(std::move(image)), alpha(GetAlpha(quality))
{}

std::unique_ptr<Image> ImageCompressor::Encode()
{
	std::unique_ptr<Image> result
		= std::unique_ptr<Image>(new Image(this->image->GetWidth(), this->image->GetHeight()));

	DCT dct(*this->image.get());
	auto compressed = dct.dct();

	// Process 8 by 8 blocks
	for (unsigned int i = 0; i < this->image->GetWidth(); i += 8)
	{
		for (unsigned int b = 0; b < this->image->GetHeight(); b += 8)
		{
			int M = (compressed->GetWidth()  - i) > 8 ? 8 : compressed->GetWidth()  - i;
			int N = (compressed->GetHeight() - b) > 8 ? 8 : compressed->GetHeight() - b;

			// Process Block
			for (int x = 0; x < M; x++)
			{
				for (int y = 0; y < N; y++)
				{
					auto pixel = compressed->GetPixel(i + x, b + y).ToYCbCr();
					pixel.y  = static_cast<uint8_t>(std::round(pixel.y  / (this->alpha * QL[x][y])));
					pixel.Cb = static_cast<uint8_t>(std::round(pixel.Cb / (this->alpha * QC[x][y])));
					pixel.Cr = static_cast<uint8_t>(std::round(pixel.Cr / (this->alpha * QC[x][y])));

					result->SetPixel(i + x, b + y, Pixel(pixel));
				} // y
			} // x
		}// b
	}// i

	return std::move(result);
}

std::unique_ptr<Image> ImageCompressor::Decode()
{
	std::unique_ptr<Image> tmp
		= std::unique_ptr<Image>(new Image(this->image->GetWidth(), this->image->GetHeight()));

	for (unsigned int i = 0; i < this->image->GetWidth(); i += 8)
	{
		for (unsigned int b = 0; b < this->image->GetHeight(); b += 8)
		{
			int M = (this->image->GetWidth()  - i) > 8 ? 8 : this->image->GetWidth()  - i;
			int N = (this->image->GetHeight() - b) > 8 ? 8 : this->image->GetHeight() - b;

			// Process Block
			for (int x = 0; x < M; x++)
			{
				for (int y = 0; y < N; y++)
				{
					auto pixel = this->image->GetPixel(i + x, b + y).ToYCbCr();
					pixel.y  = static_cast<uint8_t>(pixel.y  * (this->alpha * QL[x][y]));
					pixel.Cb = static_cast<uint8_t>(pixel.Cb * (this->alpha * QC[x][y]));
					pixel.Cr = static_cast<uint8_t>(pixel.Cr * (this->alpha * QC[x][y]));

					tmp->SetPixel(i + x, b + y, Pixel(pixel));
				} // y
			} // x
		} // b
	} // i

	DCT dct(*tmp.get());
	return dct.idct();
}

float ImageCompressor::GetAlpha(float quality)
{
	if (quality < 50.0f)
	{
		return 50.0f / (float)quality;
	}
	return 2.0f - (float)quality / 50.0f;
}
