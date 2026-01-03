#include "png/PngValueReader.hpp"
#include "Exception.hpp"

#include <iostream>

PngValueReader::PngValueReader(std::vector<uint8_t>&& decodedValues, uint8_t channelSize)
	: decodedValues(std::move(decodedValues)), channelSize(channelSize) 
{
	// TODO check of channel is 1,2,4,8,16
};


uint16_t PngValueReader::GetNextValue()
{
	// TODO check if at the end

	if (this->channelSize < 8)
	{
		auto value = this->decodedValues[this->nextByte];
		value = value >> this->nextBit;
		value = value << (8 - this->channelSize);
		value = value >> (8 - this->channelSize);

		this->nextBit += this->channelSize;
		if (this->nextBit >= 8)
		{
			this->nextByte++;
			this->nextBit = 0;
		}

		return value;
	}
	else if (this->channelSize == 8)
	{
		return this->decodedValues[this->nextByte++];
	}
	else
	{
		// TODO
		throw Exception("Not implemented yet!");
	}
}

uint8_t PngValueReader::GetFilterMethod()
{
	// TODO check if at the end

	if (this->nextBit == 0)
	{
		return this->decodedValues[this->nextByte++];
	}
	this->nextByte++;
	this->nextBit = 0;

	return this->decodedValues[this->nextByte++];
}
