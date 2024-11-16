#pragma once

#ifndef RAW_IMAGE_HPP__
#define RAW_IMAGE_HPP__

#include <string>

#include "Image.hpp"

class RAWImage : public Image
{
public:
	RAWImage(std::string filename, uint32_t width, uint32_t height);

private:
	void LoadData(std::string filename);

};

#endif /* RAW_IMAGE_HPP__ */