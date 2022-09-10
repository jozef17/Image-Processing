#pragma once

#ifndef IMAGE_COMPRESSOR_HPP__
#define IMAGE_COMPRESSOR_HPP__

#include <memory>

class Image;

class ImageCompressor
{
public:	
	ImageCompressor(std::unique_ptr<Image> image, float quality);
		
	std::unique_ptr<Image> Encode();

	std::unique_ptr<Image> Decode();

private:
	float GetAlpha(float quality);

	std::unique_ptr<Image> image;
	float alpha;

};

#endif /* IMAGE_COMPRESSOR_HPP__ */
