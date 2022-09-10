#pragma once

#ifndef COMPRESSION_HANDLER_HPP__
#define COMPRESSION_HANDLER_HPP__

#include "ArgumentHandler.hpp"
#include <memory>

class Image;

class CompressionHandler : public ArgumentHandler
{
public:
	CompressionHandler(std::unique_ptr<Image> image, float quality);

	virtual int Run() override;

private:
	std::unique_ptr<Image> originalImage;
	float quality;

};

#endif /* COMPRESSION_HANDLER_HPP__ */