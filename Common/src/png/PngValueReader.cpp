#include "png/PngValueReader.hpp"
#include "Exception.hpp"

PngValueReader::PngValueReader(std::vector<uint8_t>&& decodedValues, uint8_t channelSize)
	: decodedValues(std::move(decodedValues)), channelSize(channelSize) 
{ };

uint16_t PngValueReader::GetNextValue()
{
	if (this->nextByte >= this->decodedValues.size())
	{
		throw Exception("End of data!");
	}

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
		throw Exception("Bit size >8 is not supported!");
	}
}

uint8_t PngValueReader::GetFilterMethod()
{
	if (this->nextByte >= this->decodedValues.size())
	{
		throw Exception("End of data!");
	}

	if (this->nextBit != 0)
	{
		this->nextByte++;
		this->nextBit = 0;
	}
	return this->decodedValues[this->nextByte++];
}
