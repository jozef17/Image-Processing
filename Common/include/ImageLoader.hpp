#pragma once

#ifndef IMAGE_LOADER_HPP__
#define IMAGE_LOADER_HPP__

#include <memory>
#include <string>

class Image;

class ImageLoader final
{
public:
	ImageLoader() = delete;
	~ImageLoader() = delete;

	static std::unique_ptr<Image> LoadImage(const std::string &image);
	static std::unique_ptr<Image> LoadRawImage(const std::string &image, uint32_t width, uint32_t height);
};

#endif /* IMAGE_LOADER_HPP__ */