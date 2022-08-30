#pragma once

#ifdef USE_CUDA
#ifndef FILTER_CUH__
#define FILTER_CUH__

#include <cstdint>

#include <cuda_runtime.h>
#include <device_launch_parameters.h>

class Image;
class Kernel;

__global__ 
void ApplyFilterCuda(void* kernel, uint16_t kernelWidth, uint16_t kernelHeight,
	                 void* image,  uint32_t imageWidth,  uint32_t imageHeight,
	                 void* result);

void* CopyToGRAM(uint32_t size, void* data) noexcept;
void* CopyToGRAM(const Image& data) noexcept;
void* CopyToGRAM(const Kernel& data) noexcept;

#endif /* FILTER_CUH__ */
#endif /* USE_CUDA */