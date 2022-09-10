#pragma once

#ifndef DCT_HPP__
#define DCT_HPP__

#include <memory>

#include "Image.hpp"

// Discrete cosine transform
class DCT
{
public:
	DCT(Image &image);

	// Applies Discrete Cosine Transformation to image
	std::unique_ptr<Image> dct();

	// Applies Inverse Discrete Cosine Transformation to image
	std::unique_ptr<Image> idct();

private:

//	std::unique_ptr<Image> Apply(std::unique_ptr<Image>(void));
	// Calculate dct for 8 by 8 block
	void dctBlock(Image &result, int xOffset, int yOffset);

	// Calculate idct for 8 by 8 block
	void idctBlock(Image &result, int xOffset, int yOffset);

	Image& image;
};

#endif /* DCT_HPP__ */
