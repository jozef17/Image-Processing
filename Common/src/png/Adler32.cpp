#include "png/Adler32.hpp"

constexpr uint32_t MOD_ADLER = 65521;

uint32_t Adler32::Decode(const std::vector<uint8_t>& data)
{
	uint32_t a = 0;
	uint32_t b = 0;

	for (auto byte : data)
	{
		a = (a + byte) % MOD_ADLER;
		b = (b + a) % MOD_ADLER;
	}

	return (b << 16) | a;
}
