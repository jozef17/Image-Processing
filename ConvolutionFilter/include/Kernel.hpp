#pragma once

#ifndef KERNEL_HPP__
#define KERNEL_HPP__

#include <string>
#include <cstdint>
#include <vector>

class Kernel final
{
public:
	static Kernel GetKernel(const std::string& nameOrFile);

	uint16_t GetWidth() const noexcept;
	uint16_t GetHeight() const noexcept;

	float Get(uint16_t x, uint16_t y) const;

	std::string ToString() const noexcept;

private:
	Kernel(uint16_t width, uint16_t height, std::vector<float> kernelData);
	Kernel(std::string file);

	std::vector<float> kernel;
	uint16_t width;
	uint16_t height;

};

#endif /* KERNEL_HPP__ */