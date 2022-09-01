#include "png/HuffmanDecoder.hpp"
#include "png/BitStream.hpp"
#include "Exception.hpp"

#ifdef ENABLE_LOGS
#include <iostream>
#define LOG(msg) std::cout << msg
#else
#define LOG(msg)
#endif

HuffmanDecoder::HuffmanDecoder(BitStream& bitstream) : bitstream(bitstream)
{}

void HuffmanDecoder::Decode(std::vector<uint8_t>& data)
{
	while (true)
	{
		auto llcode = GetLLCode();
		LOG("[HuffmanDecodes::Decode] - LL Code: " << llcode << std::endl);

		if (llcode < 256)
		{
			data.push_back(static_cast<uint8_t>(llcode));
		}
		else if (llcode == 256) // end of data
		{
			break;
		}
		else
		{
			auto length = GetLength(llcode);
			auto distanceCode = GetDistanceCode();
			auto distance = GetDistance(distanceCode);
			
			LOG("[HuffmanDecodes::Decode] - Distance Code : " << distanceCode << std::endl);
			LOG("                         - Length  : " << length << std::endl);
			LOG("                         - Distance: " << distance << std::endl);

			auto start = (int)data.size() - distance;
			for (int i = 0; i < length; i++)
			{
				if ((start + i) < 0)
				{
					throw RuntimeException("Invalid distance! Start position: \'" + std::to_string(start + i) + "\' for distance: " + std::to_string((int)distance));
				}
				else
				{
					auto value = data.at(start + i);
					data.push_back(value);
				}
			}
		}
	}
}

uint16_t HuffmanDecoder::GetLength(uint32_t code)
{
	// Get number of extra bits
	uint16_t extraLookup[6] = { (uint16_t)264, (uint16_t)268, (uint16_t)272, (uint16_t)276, (uint16_t)280, (uint16_t)284 };
	uint8_t numberOfExtraBits = 0;
	for (uint8_t i = 0; i < 6; i++)
	{
		if (code <= extraLookup[i])
		{
			numberOfExtraBits = i;
			break;
		}
	}

	LOG("[HuffmanDecodes::GetLength] extra " << (int)numberOfExtraBits << "bits: ");

	// Get Extra value
	uint8_t extraValue = 0;
	for (int8_t i = 0; i < numberOfExtraBits; i++)
	{
		uint8_t extraBit = this->bitstream.GetNext();
		extraValue = (extraBit << i) | extraValue;
		LOG((int)extraBit);
	}

	LOG(", extra value: " << (int)extraValue << std::endl);

	// Get Length
	// Codes 257 - 264 --> lengths 3 - 10
	if (code >= 257 && code <= 264)
	{
		return (code - 257) + 3;
	}

	// Get start range value for code
	uint16_t startValue = 0;
	// TODO change to lookup map or something
	switch (code)
	{
	case 265:
		startValue = 11;
		break;
	case 266:
		startValue = 13;
		break;
	case 267:
		startValue = 15;
		break;
	case 268:
		startValue = 17;
		break;
	case 269:
		startValue = 19;
		break;
	case 270:
		startValue = 23;
		break;
	case 271:
		startValue = 27;
		break;
	case 272:
		startValue = 31;
		break;
	case 273:
		startValue = 35;
		break;
	case 274:
		startValue = 43;
		break;
	case 275:
		startValue = 51;
		break;
	case 276:
		startValue = 59;
		break;
	case 277:
		startValue = 67;
		break;
	case 278:
		startValue = 83;
		break;
	case 279:
		startValue = 99;
		break;
	case 280:
		startValue = 115;
		break;
	case 281:
		startValue = 131;
		break;
	case 282:
		startValue = 163;
		break;
	case 283:
		startValue = 195;
		break;
	case 284:
		startValue = 227;
		break;
	case 285:
		return 258;
	default:
		throw RuntimeException("Invalid Length Code " + std::to_string(code));
	}
	return startValue + extraValue;
}

uint16_t HuffmanDecoder::GetDistance(uint32_t distanceCode)
{
	if (distanceCode <= 3)
	{
		return distanceCode + 1;
	}

	auto numberOfExtraBits = (distanceCode - 2) / 2;
	LOG("[HuffmanDecodes::GetDistance] extra " << (int)numberOfExtraBits << "bits: ");
	
	uint16_t extra = 0;
	for (uint32_t k = 0; k < numberOfExtraBits; k++)
	{
		int extraBit = this->bitstream.GetNext();
		extra = (extraBit << k) | extra;
		LOG((int)extraBit);
	}
	LOG(", extra value: " << (int)extra << std::endl);

	// TODO refactor
	uint16_t startValue = 0;
	switch (distanceCode)
	{
	case 4:
		startValue = 5;
		break;
	case 5:
		startValue = 7;
		break;
	case 6:
		startValue = 9;
		break;
	case 7:
		startValue = 13;
		break;
	case 8:
		startValue = 17;
		break;
	case 9:
		startValue = 25;
		break;
	case 10:
		startValue = 33;
		break;
	case 11:
		startValue = 49;
		break;
	case 12:
		startValue = 65;
		break;
	case 13:
		startValue = 97;
		break;
	case 14:
		startValue = 129;
		break;
	case 15:
		startValue = 193;
		break;
	case 16:
		startValue = 257;
		break;
	case 17:
		startValue = 385;
		break;
	case 18:
		startValue = 513;
		break;
	case 19:
		startValue = 769;
		break;
	case 20:
		startValue = 1025;
		break;
	case 21:
		startValue = 1537;
		break;
	case 22:
		startValue = 2049;
		break;
	case 23:
		startValue = 3073;
		break;
	case 24:
		startValue = 4097;
		break;
	case 25:
		startValue = 6145;
		break;
	case 26:
		startValue = 8193;
		break;
	case 27:
		startValue = 12289;
		break;
	case 28:
		startValue = 16385;
		break;
	case 29:
		startValue = 24577;
		break;
	default:
		throw RuntimeException("Invalid Distance Code " + std::to_string(distanceCode));
	}

	return startValue += extra;
}
