#pragma once

#ifndef RAW_LOADER_HPP__
#define RAW_LOADER_HPP__

#include <string>
#include <memory>

#include "Image.hpp"

class RAWLoader final
{
public:
	RAWLoader(std::string filename, uint32_t width, uint32_t height);

	std::unique_ptr<Image> LoadRawImage();

private:
	std::string filename;
	uint32_t width;
	uint32_t height;
};

#endif /* RAW_LOADER_HPP__ */