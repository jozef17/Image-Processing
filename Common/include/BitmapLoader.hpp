#pragma once

#ifndef BITMAP_LOADER_HPP__
#define BITMAP_LOADER_HPP__

#include <string>
#include <memory>

class Image;

class BitmapLoader
{
public:
	BitmapLoader(std::string filename);

	/// <summary>
	/// Check if image header is aligned with bmp
	/// </summary>
	/// <param name="header">header data</param>
	/// <param name="size">size of header</param>
	/// <returns></returns>
	static bool IsBitmapImage(uint8_t *header, uint32_t size);

	/// <summary>
	/// Loads bitmap image
	/// </summary>
	/// <returns>loaded images</returns>
	std::unique_ptr<Image> LoadBitmapImage();

private:
	std::string filename;

};

#endif /* BITMAP_LOADER_HPP__ */