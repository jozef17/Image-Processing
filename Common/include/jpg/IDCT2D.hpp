#pragma once

#ifndef IDCT_HPP__
#define IDCT2D_HPP__

#include <cstdint>
#include <vector>

/// <summary>
/// Invverse 2D Discrete Cosine Transform
/// </summary>
class IDCT2D
{
public:	
	/// <summary>
	/// Apply Inverse 2D DCT on input 8x8 block
	/// </summary>
	/// <param name="inputBlock">Input block</param>
	static std::vector<double> InverseDCT(const int32_t* inputBlock);
};

#endif // IDCT_HPP__