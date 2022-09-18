#include "CompressionHandler.hpp"
#include "ImageCompressor.hpp"
#include "Window.hpp"
#include "Image.hpp"
#include "FloatImage.hpp"
#include "Pixel.hpp"

#include <algorithm>

CompressionHandler::CompressionHandler(std::unique_ptr<Image> image, float quality)
	: originalImage(std::move(image)), quality(quality)
{}

int CompressionHandler::Run()
{
	std::unique_ptr<Image> decodedImage;
	{
		// Encode
		FloatImage original(*this->originalImage.get());
		ImageCompressor encoder(original, this->quality);
		auto encoded = encoder.Encode();

		// Decode
		ImageCompressor decoder(*encoded.get(), this->quality);
		auto decoded = decoder.Decode();
		decodedImage = decoded->GetImage();
	}

	// Display image
	auto width = std::min(decodedImage->GetWidth(), (uint32_t)1'024);
	auto height = std::min(decodedImage->GetHeight(), (uint32_t)512);

	Window window(width, height);

	window.SetImage(std::move(decodedImage));
	window.SetTitle(std::to_string(this->quality)+"%");
	window.Show();

	return 0;
}