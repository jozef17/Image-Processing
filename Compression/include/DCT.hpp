#pragma once

#ifndef DCT_HPP__
#define DCT_HPP__

#include <memory>

#include "FloatImage.hpp"

// Discrete cosine transform
class DCT
{
public:
	DCT(const FloatImage &image);

	// Applies Discrete Cosine Transformation to image
	std::unique_ptr<FloatImage> dct();

	// Applies Inverse Discrete Cosine Transformation to image
	std::unique_ptr<FloatImage> idct();

private:
	// Calculate dct for 8 by 8 block
	void dctBlock(FloatImage &result, int xOffset, int yOffset);

	// Calculate idct for 8 by 8 block
	void idctBlock(FloatImage &result, int xOffset, int yOffset);

	const FloatImage &image;
};

#endif /* DCT_HPP__ */
