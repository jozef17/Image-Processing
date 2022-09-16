#include "FloatImage.hpp"
#include "Exception.hpp"

FloatImage::FloatImage(uint32_t width, uint32_t height) : width(width), height(height)
{
	this->image.resize(width * height);
}

FloatImage::FloatImage(const Image& image) : width(image.GetWidth()), height(image.GetHeight())
{
	this->image.resize(image.GetWidth() * image.GetHeight());

	for (auto i = 0u; i < image.GetHeight(); i++)
	{
		for (auto j = 0u; j < image.GetWidth(); j++)
		{
			auto pixel = image.GetPixel(j, i).ToYCbCr();
			FloatData data = { 
				static_cast<float>(pixel.y), 
				static_cast<float>(pixel.Cb),
				static_cast<float>(pixel.Cr) };
			Set(j, i, data);
		}
	}
}

std::unique_ptr<Image> FloatImage::GetImage() const
{
	std::unique_ptr<Image> img = std::unique_ptr<Image>(new Image(this->width, this->height));

	for (auto i = 0u; i < this->height; i++)
	{
		for (auto j = 0u; j < this->width; j++)
		{
			auto data = Get(j, i);
			YCbCrPixel pixel = { // TODO: TBD to 0 - 255???
				static_cast<uint8_t>(data.data[0]),
				static_cast<uint8_t>(data.data[1]),
				static_cast<uint8_t>(data.data[2]) };
			img->SetPixel(j, i, Pixel{ pixel });
		}
	}

	return std::move(img);
}


const FloatData& FloatImage::Get(uint32_t x, uint32_t y) const
{
	if (x > this->width || y > this->height)
	{
		std::string errorMessage = "Error: Get - Invalid request at [" + std::to_string(x) + ", " + std::to_string(y) +
			"] while image size is [" + std::to_string(this->width) + ", " + std::to_string(this->height) + "]";
		throw Exception(errorMessage.c_str());
	}

	return this->image[y * this->width + x];
}

void FloatImage::Set(uint32_t x, uint32_t y, float data[3])
{
	FloatData fd;
	std::memcpy(fd.data, data, 3 * sizeof(float));
	Set(x, y, fd);
}

void FloatImage::Set(uint32_t x, uint32_t y, const FloatData &data)
{
	if (x > this->width || y > this->height)
	{
		std::string errorMessage = "Error: Get - Invalid request at [" + std::to_string(x) + ", " + std::to_string(y) +
			"] while image size is [" + std::to_string(this->width) + ", " + std::to_string(this->height) + "]";
		throw Exception(errorMessage.c_str());
	}

	this->image[y * this->width + x] = data;
}

uint32_t FloatImage::GetWidth() const
{
	return this->width;
}

uint32_t FloatImage::GetHeight() const
{
	return this->height;
}
