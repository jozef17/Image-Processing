#include "CompressionHandler.hpp"
#include "ImageCompressor.hpp"
#include "Window.hpp"
#include "Image.hpp"
#include "Pixel.hpp"

#include <algorithm>

CompressionHandler::CompressionHandler(std::unique_ptr<Image> image, float quality)
	: originalImage(std::move(image)), quality(quality)
{}

int CompressionHandler::Run()
{
	std::unique_ptr<Image> decodedImage;
	{
		// Compress
		ImageCompressor encoder(std::move(this->originalImage), this->quality);
		auto encodedImage = encoder.Encode();

		ImageCompressor decoder(std::move(encodedImage), this->quality);
		decodedImage = encoder.Decode();
	}

	// Display image
	auto width = std::min(decodedImage->GetWidth(), (uint32_t)1'024);
	auto height = std::min(decodedImage->GetHeight(), (uint32_t)512);

	Window window(width, height);

	window.SetImage(std::move(decodedImage));
	window.Show();

	return 0;
}