#pragma once

#ifndef TYPE_10_DECODER_HPP__
#define TYPE_10_DECODER_HPP__

#include "BlockDecoder.hpp"

class BitStream;

class Type10Decoder final : public BlockDecoder
{
public:
	Type10Decoder(BitStream& bitstream);
	virtual ~Type10Decoder() = default;

	virtual std::vector<uint8_t> Decode() override;

private:
	// reads numOfBits and returns nuber
	uint8_t Get(uint8_t numOfBits);

	/*uint16_t GetLiteralLengthCode();
	uint8_t GetDistanceCode();

	uint16_t GetLength(uint16_t code);
	uint16_t GetDistance();*/

	BitStream& bitstream;
};

#endif /* TYPE_10_DECODER_HPP__ */
