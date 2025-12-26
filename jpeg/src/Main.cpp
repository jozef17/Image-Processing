#include "jpg/JpegLoader.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include "Window.hpp"

int main(int argc, char* argv[])
{
	std::string filename;

	filename = "D:\\Users\\User\\Pictures\\32x32.jpg";
//	filename = "D:\\Users\\User\\Pictures\\white10x10.jpg";
//	filename = "D:\\Users\\User\\Pictures\\8x8.jpg";
//	filename = "D:\\Users\\User\\Pictures\\8x8_1.jpg";

	{
		std::ifstream file(filename, std::ios::binary);
		uint8_t	buffer[128];
		while (file)
		{
			std::memset(buffer, 0, sizeof(buffer));
			file.read(reinterpret_cast<char*>(buffer), sizeof(buffer));
			for (int i = 0; i < sizeof(buffer); i++)
			{
				std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)buffer[i] << " ";
			}
		}
		std::cout << std::endl;
		std::cout << std::endl;
	}

	try
	{
		std::shared_ptr<Image>  image = std::make_shared<JpegLoader>(filename);
		Window window(32, 32);
		window.SetTitle("Jpeg Loader Test");
		window.SetImage(std::move(image));
		window.Show();
	}
	catch (std::exception &ex)
	{
		std::cerr << ex.what() << std::endl;
	}
	return 0;
}
