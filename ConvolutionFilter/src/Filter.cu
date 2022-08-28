#include "Filter.cuh"

#ifdef USE_CUDA

#include "Filter.hpp"
#include "Image.hpp"
#include "Pixel.hpp"
#include "Exception.hpp"

#include <iostream>

#define CUDA_BLOCK_COUNT 128
#define CUDA_THREAD_COUNT 128

__global__ 
void ApplyFilterCuda(void* kernel, uint16_t kernelWidth, uint16_t kernelHeight,
	                 void* image,  uint32_t imageWidth,  uint32_t imageHeight,
	                 void* res)
{
	auto imageData  = static_cast<RGBPixel*>(image);
	auto result     = static_cast<RGBPixel*>(res);
	auto kernelData = static_cast<float*>(kernel);

	uint32_t block  = blockIdx.x;
	uint32_t thread = threadIdx.x;

	// a & b loops go through final image
	for (uint32_t a = block; a < imageHeight; a += CUDA_BLOCK_COUNT) // y
	{
		for (uint32_t b = thread; b < imageWidth; b+= CUDA_THREAD_COUNT) // x
		{
			float red = 0;
			float green = 0;
			float blue = 0;

			for (int c = 0; c < kernelHeight; c++) // y
			{
				for (int d = 0; d < kernelWidth; d++) // x
				{
					auto y = c - kernelHeight / 2;
					y = (kernelHeight + a + y) % imageHeight;

					auto x = d - kernelWidth / 2;
					x = (kernelWidth + b + x) % imageWidth;
				
					auto kernelVal = kernelData[c * kernelWidth + d];
					auto imageVal = imageData[y * imageWidth + x];
					if(kernelVal > 0)

					red   += kernelVal * imageVal.red;
					green += kernelVal * imageVal.green;
					blue  += kernelVal * imageVal.blue;
				}
			}

			red   = red   > 0 ? (red   > 255 ? 255 : red)   : 0;
			green = green > 0 ? (green > 255 ? 255 : green) : 0;
			blue  = blue  > 0 ? (blue  > 255 ? 255 : blue)  : 0;

			result[a * imageWidth + b].red   = static_cast<uint8_t>(red);
			result[a * imageWidth + b].green = static_cast<uint8_t>(green);
			result[a * imageWidth + b].blue  = static_cast<uint8_t>(blue);
		}
	}
}

void* CopyToGRAM(uint32_t size, void* data) noexcept
{
	void* gpuData = nullptr;
	cudaError_t error;

	// Alocate GPU memory
	error = cudaMalloc(&gpuData, size);
	if (error != cudaSuccess)
	{
		std::cerr << "Error: Unable to allocate GRAM of size: " << size << std::endl;
		cudaFree(gpuData);
		return nullptr;
	}
	cudaDeviceSynchronize();

	// Copy Data to GPU memory
	error = cudaMemcpy(gpuData, data, size, cudaMemcpyHostToDevice);
	if (error != cudaSuccess)
	{
		std::cerr << "Error: Unable to copy data to GRAM of size: " << size << std::endl;
		cudaFree(gpuData);
		return nullptr;
	}
	cudaDeviceSynchronize();
	return gpuData;
}

void* CopyToGRAM(const Image& data) noexcept
{
	auto size = data.GetWidth() * data.GetHeight();

	std::unique_ptr<RGBPixel[]> imageData = std::unique_ptr<RGBPixel[]>(new RGBPixel[size]);

	// Prepare data
	uint64_t location = 0;
	for (uint32_t i = 0; i < data.GetHeight(); i++)
	{
		for (uint32_t j = 0; j < data.GetWidth(); j++)
		{
			auto pixel = data.GetPixel(j, i).ToRGB();
			imageData[location].red   = pixel.red;
			imageData[location].green = pixel.green;
			imageData[location].blue  = pixel.blue;
			location++;
		}
	}
	// Copy to GRAM
	return CopyToGRAM(static_cast<uint32_t>(size * 3), imageData.get());
}

void* CopyToGRAM(const Kernel& data) noexcept
{
	auto size = data.GetWidth() * data.GetHeight();

	std::unique_ptr<float[]> kernelData  = std::unique_ptr<float[]>(new float[size]);
	
	uint32_t location = 0;
	for (uint16_t i = 0; i < data.GetHeight(); i++)
	{
		for (uint16_t j = 0; j < data.GetWidth(); j++)
		{
			kernelData[location++] = data.Get(j, i);
		}
	}
	// Copy to GRAM
	return CopyToGRAM(static_cast<uint32_t>(size * sizeof(float)), kernelData.get());
}

std::shared_ptr<Image> Filter::ApplyFilter()
{
	bool error = false;
	std::shared_ptr<Image> result = std::shared_ptr<Image>(new Image(this->image->GetWidth(), this->image->GetHeight()));

	// Copy data to GPU memory
	void* cudaKernel = CopyToGRAM(this->kernel);
	void* cudaImage  = CopyToGRAM(*this->image.get());
	void* cudaResult = nullptr;

	cudaMalloc(&cudaResult, this->image->GetHeight() * this->image->GetWidth() * 3);
	cudaDeviceSynchronize();

	if ((cudaKernel != nullptr) && (cudaImage != nullptr) && (cudaResult!=nullptr))
	{
		ApplyFilterCuda<<<CUDA_BLOCK_COUNT, CUDA_THREAD_COUNT >>>
			(cudaKernel, this->kernel.GetWidth(), this->kernel.GetHeight(),
			  cudaImage, this->image->GetWidth(), this->image->GetHeight(),
			 cudaResult);
		cudaDeviceSynchronize();

		// Copy result image back from gram
		auto imageSize = this->image->GetWidth() * this->image->GetHeight();
		std::unique_ptr<RGBPixel[]> imageData =
			std::unique_ptr<RGBPixel[]>(new RGBPixel[imageSize]);
		cudaMemcpy(imageData.get(), cudaResult, imageSize * 3, cudaMemcpyDeviceToHost);
		cudaDeviceSynchronize();

		// Postprocess data
		uint64_t loc = 0;
		for (uint32_t i = 0; i < this->image->GetHeight(); i++)
		{
			for (uint32_t j = 0; j < this->image->GetWidth(); j++)
			{
				// Set Pixel
				Pixel pixel = { imageData[loc++] };
				result->SetPixel(j, i, pixel);
			}
		}
	}
	else
	{
		error = true;
	}

	// Cleanup
	if (cudaKernel != nullptr)
	{
		cudaFree(cudaKernel);
	}

	if (cudaImage != nullptr)
	{
		cudaFree(cudaImage);
	}

	if (cudaResult != nullptr)
	{
		cudaFree(cudaResult);
	}

	if (error)
	{
		throw RuntimeException("Error: Unable to allocate data on GPU!");
	}

	return result;
}

#endif /* USE_CUDA */