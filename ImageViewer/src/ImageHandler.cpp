#include "ImageHandler.hpp"
#include "Image.hpp"
#include "ImageLoader.hpp"
#include "Window.hpp"
#include "Pixel.hpp"
#include "Utils.hpp"
#include "Exception.hpp"

#include <algorithm>

ImageHandler::ImageHandler(std::string filename) : filename(filename)
{
	this->image = ImageLoader::LoadImage(filename);
}

ImageHandler::ImageHandler(std::string filename, uint32_t width, uint32_t height) : filename(filename)
{
	this->image = ImageLoader::LoadRawImage(filename, width, height);
}

ImageHandler::~ImageHandler()
{}

int ImageHandler::Run()
{
	auto width  = std::min(this->image->GetWidth(), (uint32_t)1'024);
	auto height = std::min(this->image->GetHeight(), (uint32_t)512);

	Window window(width,height);

	window.SetImage(this->image);
	window.SetTitle(this->filename);
	window.Show();

	return 0;
}
