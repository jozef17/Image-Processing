#pragma once

#ifndef BLOCK_DECODER_HPP__
#define BLOCK_DECODER_HPP__

#include <vector>

class BlockDecoder
{
public:
	virtual ~BlockDecoder() {};

	virtual std::vector<uint8_t> Decode() = 0;

protected:
	BlockDecoder() = default;

};

#endif /* BLOCK_DECODER_HPP__ */