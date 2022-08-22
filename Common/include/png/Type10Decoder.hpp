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

	uint16_t Get(const std::vector<Code>& codes);

	// decodes code lengths
	std::vector<Code> GetCodeLength(uint8_t hclen);

	// decodes literal length codes
	std::vector<Code> GetLiteralLengthCodes(uint8_t hlit, const std::vector<Code> &distCodes);

	// decodes distance codes
	std::vector<Code> GetDistanceCodes(uint8_t hdist, const std::vector<Code> &distCodes);

	/*uint16_t GetLiteralLengthCode();
	uint8_t GetDistanceCode();

	uint16_t GetLength(uint16_t code);
	uint16_t GetDistance();*/

	BitStream& bitstream;
};

#endif /* TYPE_10_DECODER_HPP__ */
