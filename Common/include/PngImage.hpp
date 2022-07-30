#pragma once

#ifndef PNG_IMAGE_HPP__
#define PNG_IMAGE_HPP__

#include <string>

#include "Image.hpp"

class PngImage : public Image
{
public:
	PngImage(std::string filename);

private:
	void LoadData(std::string filename);

};

#endif /* PNG_IMAGE_HPP__ */