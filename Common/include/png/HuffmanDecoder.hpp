#pragma once

#ifndef HUFFMAN_DECODER_HPP__
#define HUFFMAN_DECODER_HPP__

#include "BlockDecoder.hpp"
class BitStream;

class HuffmanDecoder : public BlockDecoder
{
public:
	virtual void Decode(std::vector<uint8_t>& data) override;
	virtual ~HuffmanDecoder() {};

protected:
	HuffmanDecoder(BitStream& bitstream);

	virtual uint16_t GetLLCode() = 0;
	virtual uint16_t GetDistanceCode() = 0;

	BitStream& bitstream;

private:
	uint16_t GetLength(uint32_t code);
	uint16_t GetDistance(uint32_t code);

};

#endif /* HUFFMAN_DECODER_HPP__ */