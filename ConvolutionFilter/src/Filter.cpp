#include "Filter.hpp"
#include "Image.hpp"
#include "Pixel.hpp"

#include <thread>
#include <functional>

Filter::Filter(std::shared_ptr<Image> image, Kernel kernel) noexcept : image(image), kernel(kernel)
{}

Filter::Filter(const Filter& old)
	: image(old.image), kernel(old.kernel)
{}

Filter& Filter::operator=(const class Filter& other)
{
	this->image = other.image;
	this->kernel = other.kernel;
	return *this;
}

Filter::~Filter()
{}

std::shared_ptr<Image> Filter::ApplyFilter()
{
	std::shared_ptr<Image> result = std::shared_ptr<Image>(new Image(this->image->GetWidth(), this->image->GetHeight()));

	const int threadCount = 8;
	auto applyFilterFunct = [&](int id) -> void
	{
		// a & b loops go through final image
		for (uint32_t a = id; a < this->image->GetHeight(); a += threadCount) // y
		{
			for (uint32_t b = 0; b < this->image->GetWidth(); b++) // x
			{
				float red   = 0;
				float green = 0;
				float blue  = 0;

				for (int c = 0; c < this->kernel.GetHeight(); c++) // y
				{
					for (int d = 0; d < this->kernel.GetWidth(); d++) // x
					{
						auto y = c - this->kernel.GetHeight() / 2;
						y = (this->image->GetHeight() + a + y) % this->image->GetHeight();

						auto x = d - this->kernel.GetWidth() / 2;
						x = (this->image->GetWidth() + b + x) % this->image->GetWidth();

						auto kernelVal = this->kernel.Get(d, c);
						auto imageVal = this->image->GetPixel(x, y).ToRGB();

						red += kernelVal * imageVal.red;
						green += kernelVal * imageVal.green;
						blue += kernelVal * imageVal.blue;
					}
				}

				// Trim values to 0 - 255 range
				red   = std::max(std::min(red, 255.f), 0.f);
				green = std::max(std::min(green, 255.f), 0.f);
				blue  = std::max(std::min(blue, 255.f), 0.f);

				// Set Pixel
				Pixel pixel = { RGBPixel{ (uint8_t)red, (uint8_t)green, (uint8_t)blue } };
				result->SetPixel(b, a, pixel);
			}
		}
	};
	
	// Run filter in parallel
	std::thread threads[threadCount - 1];
	for (int i = 0; i < threadCount - 1; i++)
	{
		threads[i] = std::thread(applyFilterFunct, i);
	}

	// Make use of main thread
	applyFilterFunct(threadCount - 1);

	// Wait for threads to finish
	for (int i = 0; i < threadCount - 1; i++)
	{
		threads[i].join();
	}

	return result;
}
