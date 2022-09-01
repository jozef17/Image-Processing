#pragma once

#ifndef TYPE_01_DECODER_HPP__
#define TYPE_01_DECODER_HPP__

#include "HuffmanDecoder.hpp"

class BitStream;

class Type01Decoder final : public HuffmanDecoder
{
public:
	Type01Decoder(BitStream& bitstream);
	virtual ~Type01Decoder() = default;

protected: 
	virtual uint16_t GetLLCode() override;
	virtual uint16_t GetDistanceCode() override;

};

#endif /* TYPE_01_DECODER_HPP__ */


