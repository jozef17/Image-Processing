#include "ImageHandler.hpp"
#include "RAWImage.hpp"
#include "BitmapImage.hpp"
#include "Window.hpp"
#include "Pixel.hpp"
#include "Utils.hpp"
#include "Exception.hpp"
#include "png/PngImage.hpp"

#include <algorithm>

ImageHandler::ImageHandler(std::string filename) : filename(filename)
{
	auto lowercaseFile = Utils::ToLowercase(filename);
	if (Utils::EndsWith(lowercaseFile, ".png"))
	{
		this->image = std::shared_ptr<Image>(new PngImage(filename));
	}
	else if (Utils::EndsWith(lowercaseFile, ".bmp"))
	{
		this->image = std::shared_ptr<Image>(new BitmapImage(filename));
	}
	else
	{
		throw RuntimeException("Unsupported Image format file: \""+filename+"\"! Only png, bmp and raw images are supported!");
	}
}

ImageHandler::ImageHandler(std::string filename, uint32_t width, uint32_t height) : filename(filename)
{
	this->image = std::shared_ptr<Image>(new RAWImage(filename, width, height));
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
