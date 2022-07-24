#include "ImageHandler.hpp"
#include "RAWImage.hpp"
#include "BitmapImage.hpp"
#include "Window.hpp"
#include "Pixel.hpp"

#include <algorithm>

ImageHandler::ImageHandler(std::string filename) : filename(filename)
{
	this->image = std::unique_ptr<Image>(new BitmapImage(filename));
}

ImageHandler::ImageHandler(std::string filename, uint32_t width, uint32_t height) : filename(filename)
{
	this->image = std::unique_ptr<Image>(new RAWImage(filename, width, height));
}

ImageHandler::~ImageHandler()
{}

int ImageHandler::Run()
{
	auto width  = std::min(this->image->GetWidth(), (uint32_t)1'024);
	auto height = std::min(this->image->GetHeight(), (uint32_t)512);

	Window window(width,height);

	window.SetImage(std::move(this->image));
	window.SetTitle(this->filename);
	window.Show();

	return 0;
}
