#pragma once

#ifndef TYPE_01_DECODER_HPP__
#define TYPE_01_DECODER_HPP__

#include "BlockDecoder.hpp"

class BitStream;

class Type01Decoder final : public BlockDecoder
{
public:
	Type01Decoder(BitStream& bitstream);
	virtual ~Type01Decoder() = default;

	virtual void Decode(std::vector<uint8_t> &data) override;

private:
	uint16_t GetLiteralLengthCode();
	uint8_t GetDistanceCode();

	uint16_t GetLength(uint16_t code);
	uint16_t GetDistance();

	BitStream& bitstream;
};

#endif /* TYPE_01_DECODER_HPP__ */


