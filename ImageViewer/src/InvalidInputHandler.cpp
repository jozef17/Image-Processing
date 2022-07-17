#include "InvalidInputHandler.hpp"
#include <iostream>

int InvalidInputHandler::Run()
{
	std::cout << "Usage: ImageViewer [--file] <filepath>" << std::endl;
	std::cout << "       ImageViewer [--file] <filepath> <image-width> <image-height>" << std::endl;
	std::cout << "       ImageViewer [--help]" << std::endl;

	return 0;
}
