#pragma once

#ifndef FLOAT_IMAGE_HPP__
#define FLOAT_IMAGE_HPP__

#include "Image.hpp"
#include <vector>
#include <memory>

struct FloatData
{
	float data[3] = { 0,0,0 };
};

class FloatImage
{
public:
	FloatImage(uint32_t width, uint32_t height);
	FloatImage(const Image& image);

	std::unique_ptr<Image> GetImage() const;

	const FloatData &Get(uint32_t x, uint32_t y) const;
	void Set(uint32_t x, uint32_t y, float data[3]);
	void Set(uint32_t x, uint32_t y, const FloatData &data);

	uint32_t GetWidth() const;
	uint32_t GetHeight() const;

private:
	std::vector<FloatData> image;

	uint32_t width;
	uint32_t height;

};

#endif /* FLOAT_IMAGE_HPP__ */