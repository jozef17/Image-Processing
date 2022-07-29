#include "FilterHandler.hpp"
#include "Window.hpp"
#include "Image.hpp"
#include "Pixel.hpp"

#include <algorithm>

FilterHandler::FilterHandler(Filter filter) noexcept : filter(std::move(filter))
{}

FilterHandler::~FilterHandler()
{}

int FilterHandler::Run()
{
	auto image = filter.ApplyFilter();

	auto width = std::min(image->GetWidth(), (uint32_t)1'024);
	auto height = std::min(image->GetHeight(), (uint32_t)512);

	Window window(width, height);

	window.SetImage(image);
	window.Show();

	return 0;
}
