#include "Kernel.hpp"
#include "Exception.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>

Kernel::Kernel(uint16_t width, uint16_t height, std::vector<float> kernelData) 
	: width(width), height(height), kernel(kernelData)
{}

Kernel::Kernel(const std::string &filename)
{
	std::ifstream file(filename);
	if (!file.is_open())
	{
		throw RuntimeException("Unable to open file: \"" + filename + "\"");
	}

	std::string in;
	while (!file.eof())
	{
		file >> in;
		try 
		{
			this->kernel.push_back(std::stof(in));
		}
		catch (std::exception)
		{
			std::string errMsg = "Invalid file formating in \"" + filename + "\". Input no. " + std::to_string(this->kernel.size()+1) +
				": \"" + in + "\" is not a number";
			throw RuntimeException(errMsg);
		}
	}

	this->width = static_cast<uint16_t>(std::sqrt(this->kernel.size()));
	this->height = this->width;
}

Kernel Kernel::GetKernel(const std::string& nameOrFile)
{
	// Convert to uppercase
	std::string uppercase = nameOrFile;
	std::transform(uppercase.begin(), uppercase.end(), uppercase.begin(), ::toupper);
	
	if (uppercase == "BOXBLUR")
	{
		return Kernel(3, 3, std::vector<float>{ 1.f / 9.f, 1.f / 9.f, 1.f / 9.f,
												1.f / 9.f, 1.f / 9.f, 1.f / 9.f,
												1.f / 9.f, 1.f / 9.f, 1.f / 9.f });
	}
	else if (uppercase == "EDGEDETECTIONH")
	{
		return Kernel(3, 3, std::vector<float>{ 1,  1,  1,
												0,  0,  0,
											   -1, -1, -1 });
	}
	else if (uppercase == "EDGEDETECTIONV")
	{
		return Kernel(3, 3, std::vector<float>{ 1, 0, -1,
												1, 0, -1,
												1, 0, -1 });
	}
	else if (uppercase == "GAUSSIAN")
	{
		return Kernel(3, 3, std::vector<float>{ 1.f / 16.f, 2.f / 16.f, 1.f / 16.f,
												2.f / 16.f, 4.f / 16.f, 2.f / 16.f,
												1.f / 16.f, 2.f / 16.f, 1.f / 16.f });
	}
	else if (uppercase == "GAUSSIAN5")
	{
		return Kernel(5, 5, std::vector<float>{ 1.f / 256.f,  4.f / 256.f,  6.f / 256.f,  4.f / 256.f, 1.f / 256.f,
												4.f / 256.f, 16.f / 256.f, 24.f / 256.f, 16.f / 256.f, 4.f / 256.f,
												6.f / 256.f, 24.f / 256.f, 36.f / 256.f, 24.f / 256.f, 6.f / 256.f,
												4.f / 256.f, 16.f / 256.f, 24.f / 256.f, 16.f / 256.f, 4.f / 256.f,
												1.f / 256.f,  4.f / 256.f,  6.f / 256.f,  4.f / 256.f, 1.f / 256.f });
	}
	else if (uppercase == "IDENTITY")
	{
		return Kernel(3, 3, std::vector<float>{ 0, 0, 0,
												0, 1, 0,
												0, 0, 0 });
	}
	else if (uppercase == "LAPLACIAN")
	{
		return Kernel(3, 3, std::vector<float>{ 0,  1, 0,
												1, -4, 1,
												0,  1, 0 });
	}
	else if (uppercase == "SHARPEN")
	{
		return Kernel(3, 3, std::vector<float>{  0, -1,  0,
												-1,  5, -1,
												 0, -1,  0 });
	}
	else if (uppercase == "SOBEL")
	{
		return Kernel(3, 3, std::vector<float>{ 1, 0, -1,
												2, 0, -2,
												1, 0, -1 });
	}
	
	return Kernel(nameOrFile);
}

uint16_t Kernel::GetWidth() const noexcept
{
	return this->width;
}

uint16_t Kernel::GetHeight() const noexcept
{
	return this->height;
}

float Kernel::Get(uint16_t x, uint16_t y) const
{
	if (x > this->width || y > this->height)
	{
		throw Exception("Invalid kernel coordinates: [" + std::to_string(x) + "," + std::to_string(y) +
			"], while kernel size is: [" + std::to_string(this->width) + "," + std::to_string(this->height) + "]"
		);
	}

	return this->kernel.at(this->width * y + x);
}

std::string Kernel::ToString() const noexcept
{
	std::stringstream val;

	for (int i = 0; i < this->height; i++)
	{
		for (int j = 0; j < this->width; j++)
		{
			val << std::setfill(' ') << std::setw(6) << std::setprecision(2) << std::fixed << Get(j, i) << " ";
		}
		val << "\n";
	}

	return val.str();
}
