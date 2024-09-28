#include "jpg/JpegImage.hpp"
#include <iostream>

int main(int argc, char* argv[])
{
	try{
	JpegImage image("D:\\Users\\User\\Pictures\\white10x10.jpg");
//	JpegImage image("D:\\Users\\User\\Pictures\\yellow1.jpg");
	}
	catch (std::exception &ex)
	{
		std::cerr << ex.what() << std::endl;
	}
	return 0;
}
