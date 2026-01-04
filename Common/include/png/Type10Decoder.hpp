#pragma once

#ifndef TYPE_10_DECODER_HPP__
#define TYPE_10_DECODER_HPP__

#include <vector>

#include "HuffmanDecoder.hpp"
#include "HuffmanCode.hpp"

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
	uint16_t Get(const std::vector<Code>& codes);

	// decodes code lengths
	std::vector<Code> GetCodeLengths(uint8_t hclen);

	// decodes alphabet
	std::vector<Code> GetAlphabet(uint16_t numElements, const std::vector<Code> &distCodes);

	std::vector<Code> llAphabet;
	std::vector<Code> distanceAlphabet;
};

#endif /* TYPE_10_DECODER_HPP__ */
