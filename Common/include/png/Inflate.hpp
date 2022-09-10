#pragma once

#ifndef INFLATE_HPP__
#define INFLATE_HPP__

#include <vector>
#include <cstdint>
#include <memory>

class BitStream;
class BlockDecoder;

class Inflate final
{
public:
	Inflate(BitStream &bitstream) noexcept;

	std::vector<uint8_t> Decode();

private:
	std::unique_ptr<BlockDecoder> GetDecoder();

	BitStream& bitstream;
};

#endif /* INFLATE_HPP__ */