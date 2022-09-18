#pragma once

#ifndef IMAGE_COMPRESSOR_HPP__
#define IMAGE_COMPRESSOR_HPP__

#include "FloatImage.hpp"
#include <memory>

class ImageCompressor
{
public:	
	ImageCompressor(const FloatImage &image, float quality);
		
	std::unique_ptr<FloatImage> Encode();

	std::unique_ptr<FloatImage> Decode();

private:
	float GetAlpha(float quality);

	const FloatImage& image;
	float alpha;

};

#endif /* IMAGE_COMPRESSOR_HPP__ */
