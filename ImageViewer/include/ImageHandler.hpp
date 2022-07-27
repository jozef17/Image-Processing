#pragma once

#ifndef IMAGE_HANDLER_HPP__
#define IMAGE_HANDLER_HPP__

#include "ArgumentHandler.hpp"

#include <string>
#include <memory>
#include <cstdint>

class Image;

class ImageHandler final : public ArgumentHandler
{
public:
	ImageHandler(std::string filename);
	ImageHandler(std::string filename, uint32_t width, uint32_t height);
	~ImageHandler();

	virtual int Run() override;

private:
	std::shared_ptr<Image> image;
	std::string filename;

};


#endif /* IMAGE_HANDLER_HPP__ */
