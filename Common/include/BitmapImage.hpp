#pragma once

#ifndef BITMAP_IMAGE_HPP__
#define BITMAP_IMAGE_HPP__

#include <string>

#include "Image.hpp"

class BitmapImage : public Image
{
public:
	BitmapImage(std::string filename);

};

#endif /* BITMAP_IMAGE_HPP__ */