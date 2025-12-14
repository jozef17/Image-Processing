#include "jpg/JpegLoader.hpp"
#include <iostream>
#include "Window.hpp"

int main(int argc, char* argv[])
{
	try{
		std::shared_ptr<Image>  image = std::make_shared<JpegLoader>("D:\\Users\\User\\Pictures\\white10x10.jpg");
//	JpegLoader image("D:\\Users\\User\\Pictures\\white10x10.jpg");
		Window window(32, 32);
		window.SetTitle("Jpeg Loader Test");
		window.SetImage(std::move(image));
		window.Show();
//	JpegImage image("D:\\Users\\User\\Pictures\\yellow1.jpg");
	}
	catch (std::exception &ex)
	{
		std::cerr << ex.what() << std::endl;
	}
	return 0;
}
