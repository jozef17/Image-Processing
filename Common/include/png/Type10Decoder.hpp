#pragma once

#ifndef TYPE_10_DECODER_HPP__
#define TYPE_10_DECODER_HPP__

#include <vector>

#include "HuffmanDecoder.hpp"

struct HuffmanCode;

class Type10Decoder final : public HuffmanDecoder
{
public:
	Type10Decoder(BitStream& bitstream);
	virtual ~Type10Decoder();

protected:
	virtual uint16_t GetLLCode() override;
	virtual uint16_t GetDistanceCode() override;

private:
	// reads numOfBits and returns nuber
	uint16_t Get(uint8_t numOfBits);

	// Decodes bits and returns code based on alphabet
	uint16_t Get(const std::vector<HuffmanCode>& codes);

	// decodes code lengths
	std::vector<HuffmanCode> GetCodeLengths(uint8_t hclen);

	// decodes alphabet
	std::vector<HuffmanCode> GetAlphabet(uint16_t numElements, const std::vector<HuffmanCode> &distCodes);

	std::vector<HuffmanCode> llAphabet;
	std::vector<HuffmanCode> distanceAlphabet;
};

#endif /* TYPE_10_DECODER_HPP__ */
