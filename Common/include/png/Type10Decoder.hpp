#pragma once

#ifndef TYPE_10_DECODER_HPP__
#define TYPE_10_DECODER_HPP__

#include "BlockDecoder.hpp"

struct Code;
class BitStream;

class Type10Decoder final : public BlockDecoder
{
public:
	Type10Decoder(BitStream& bitstream);
	virtual ~Type10Decoder() = default;

	virtual std::vector<uint8_t> Decode() override;

private:
	// reads numOfBits and returns nuber
	uint16_t Get(uint8_t numOfBits);

	// Decodes bits and returns code based on alphabet
	uint16_t Get(const std::vector<Code>& codes);

	// decodes code lengths
	std::vector<Code> GetCodeLengths(uint8_t hclen);

	// decodes alphabet
	std::vector<Code> GetAlphabet(uint16_t numElements, const std::vector<Code> &distCodes);

	uint16_t GetDistance(uint32_t distanceCode);
	uint16_t GetLength(uint16_t code);

	BitStream& bitstream;
};

#endif /* TYPE_10_DECODER_HPP__ */
