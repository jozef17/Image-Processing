#include "HelpHandler.hpp"
#include <iostream>

int HelpHandler::Run()
{
	std::cout << "Usage: ImageViewer [OPTION...] [FILE...]" << std::endl;
	std::cout << "Displays image passed as input" << std::endl << std::endl;
	std::cout << "Mandatory arguments to long options are mandatory for short options too." << std::endl;
	std::cout << "  -f. --file                 display image passed as argument. For raw images, image file is followed" << std::endl;
	std::cout << "                             by image width and image height." << std::endl;
	std::cout << "  -h, --help                 display this help and exit" << std::endl;

	return 0;
}
