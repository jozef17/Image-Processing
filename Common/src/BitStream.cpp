#include "BitStream.hpp"
#include "Exception.hpp"

#ifdef ENABLE_LOGS
#include <sstream>
#include <iomanip>
#endif

bool BitStream::GetNext()
{
	if (this->arrayPosition >= this->data.size())
	{
		throw RuntimeException("Error: End of stream");
	}

	auto byte = this->data[this->arrayPosition][this->bytePosition];
	auto bit = byte & 1 << this->bitPosition++;

	// Check if last bit of byte
	if (this->bitPosition >= 8)
	{
		this->bitPosition = 0;
		this->bytePosition++;
		// Check if last byte of array
		if (this->bytePosition >= this->lengths[this->arrayPosition])
		{
			this->bytePosition = 0;
			this->arrayPosition++;
		}
	}

	return bit > 0 ? true : false;
}

uint8_t BitStream::GetCurrentByte()
{
	if (this->arrayPosition >= this->data.size())
	{
		throw RuntimeException("Error: End of stream");
	}

	auto byte = this->data[this->arrayPosition][this->bytePosition++];

	this->bitPosition = 0; // reset bit
	// Check if last byte of array
	if (this->bytePosition >= this->lengths[this->arrayPosition])
	{
		this->bytePosition = 0;
		this->arrayPosition++;
	}
	return byte;
}

bool& BitStream::operator>>(bool& b)
{
	b = GetNext();
	return b;
}

uint8_t& BitStream::operator>>(uint8_t& b)
{
	b = GetCurrentByte();
	return b;
}

void BitStream::Skip(uint32_t bitsToSkip)
{
	// TODO optimise
	for (uint32_t i = 0; i < bitsToSkip; i++)
	{
		GetNext();
	}
}

void BitStream::Append(std::unique_ptr<uint8_t[]> data, uint32_t size)
{
	this->data.push_back(std::move(data));
	this->lengths.push_back(size);
}

#ifdef ENABLE_LOGS
std::string BitStream::LogCurrent()
{
	std::stringstream s;
	auto byte = this->data[this->arrayPosition][this->bytePosition];

	s << "Array pos #" << (int)this->arrayPosition << "/" << this->data.size() << "\n";
	s << "Byte pos  #" << (int)this->bytePosition << "/" << this->lengths[this->arrayPosition] << "\n";
	s << "Bite pos  #" << (int)this->bitPosition << "/8\n";
	s << "\t" << std::setfill('0') << std::hex << (int)byte << " ";
	for (uint8_t i = 0; i < 8; i++)
	{
		s << ((byte & 1 << i) ? "1" : "0");
	}
	s << "\n";

	return s.str();
}
#endif
