#include "png/Type01Decoder.hpp"
#include "png/BitStream.hpp"
#include "Exception.hpp"

#include <iostream>
#include <iomanip>
#include <bitset>

Type01Decoder::Type01Decoder(BitStream& bitstream) : bitstream(bitstream)
{}

std::vector<uint8_t> Type01Decoder::Decode()
{
	std::vector<uint8_t> data;
	// TODO verify order of bites!!!
	std::cout << "Compressed with fixed codes" << std::endl;

	while (true)
	{
		auto literalLengthCode = GetLiteralLengthCode();

		// Check if literal/length code is valid
		if ((literalLengthCode == 286) || (literalLengthCode == 287))
		{
			throw RuntimeException("Invalid length code " + std::to_string(literalLengthCode));
		}

		// Code is literal
		if (literalLengthCode < 256)
		{
			data.push_back(static_cast<uint8_t>(literalLengthCode));
			continue;
		}
		// Code is "end of block"
		if (literalLengthCode == 256)
		{
			break;
		}

		// Code is length

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		std::cout << "*****************************************************************************" << std::endl;/////////////////////////////		
		//		std::cout << "decode distance from input stream move backwards distance bytes in the output"
		//		<< " stream, and copy length bytes from this position to the output stream." << std::endl;
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		auto length = GetLength(literalLengthCode);
		auto distance = GetDistance();
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		std::cout << "length: " << std::setw(3) << std::setfill('0') << std::dec << length << ", distance: " << (int)distance << std::endl;/////////
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		auto start = (int)data.size() - (int)distance;
		for (int i = 0; i < length; i++)
		{
			if ((start + i) < 0)// TODO ????
			{
				std::cout << "   !!! Adding:" << (int)0 << std::endl;
				data.push_back(0);
			}
			else
			{
				auto value = data.at(start + i);
				std::cout << "   Adding:" << (int)value << std::endl;
				data.push_back(value);
			}
		}
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		std::cout << "*****************************************************************************" << std::endl;/////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		/*
		auto distBit4 = bitstream.GetNext();
		auto distBit3 = bitstream.GetNext();
		auto distBit2 = bitstream.GetNext();
		auto distBit1 = bitstream.GetNext();
		auto distBit0 = bitstream.GetNext();

		uint32_t distCode = (int)distBit0 | ((int)distBit1 << 1) | ((int)distBit2 << 2) | ((int)distBit3 << 3) | ((int)distBit4 << 4);

		if ((distCode == 30) || (distCode == 31))
		{
			throw RuntimeException("Invalid distance code " + std::to_string(distCode));
		}

		auto distance = distCode;
		if (distCode > 3)
		{
			auto c = (distCode - 2) / 2;

			int extra = 0;
			for (auto k = 0; k < c; k++)
			{
				int a = bitstream.GetNext();
				extra = extra | (a << k);
			}
			std::cout << "Distance code " << distCode << ", extra: " << c << ", extra value: " << extra << std::endl;
			distance += extra;
		}
		std::cout << "Distance: " << distance << std::endl;

		std::cout << "---------------------------------------------------------------------------" << std::endl;
		for (auto a : data)
		{
			std::cout << (int)a << " ";
		}
		std::cout << std::endl;
		std::cout << "---------------------------------------------------------------------------" << std::endl;
		*/
	}
	return data;
}

uint16_t Type01Decoder::GetLiteralLengthCode()
{
	// Read first 7 bits
	auto bit6 = bitstream.GetNext();
	auto bit5 = bitstream.GetNext();
	auto bit4 = bitstream.GetNext();
	auto bit3 = bitstream.GetNext();
	auto bit2 = bitstream.GetNext();
	auto bit1 = bitstream.GetNext();
	auto bit0 = bitstream.GetNext();

	uint16_t code = (int)bit0 | ((int)bit1 << 1) | ((int)bit2 << 2) | ((int)bit3 << 3) | ((int)bit4 << 4) | ((int)bit5 << 5) | ((int)bit6 << 6);
	if ((code <= 23)) // 7bit codes: 256 - 279 !bit6 && !bit5 &&
	{
		code += 256;
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		std::cout << std::setw(3) << std::setfill('0') << std::dec << (int)code << "     " << bit6 << bit5 << bit4 << bit3 << bit2 << bit1 << bit0 << std::endl;/////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		return code;
	}

	// Get 8th bit
	auto bit7 = bitstream.GetNext();
	code = (code << 1) | ((int)bit7);
	if ((code >= 48) && (code <= 191)) // 8bit codes: 0 - 143
	{
		code -= 48;
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		std::cout << std::setw(3) << std::setfill('0') << std::dec << (int)code << "    "  << bit6 << bit5 << bit4 << bit3 << bit2 << bit1 << bit0 << bit7 << std::endl;//////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		return code;
	}
	else if ((code >= 192) && (code <= 199)) // 8bit codes: 280 - 287
	{
		code -= 88;
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		std::cout << std::setw(3) << std::setfill('0') << std::dec << (int)code << "    "  << bit6 << bit5 << bit4 << bit3 << bit2 << bit1 << bit0 <<  bit7 << std::endl;//////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		return code;
	}

	// 9bit 144 - 255
	auto bit8 = bitstream.GetNext();
	code = (code << 1)| ((int)bit8);
	code -= 256;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	std::cout << std::setw(3) << std::setfill('0') << std::dec << (int)code << "   "  << bit6 << bit5 << bit4 << bit3 << bit2 << bit1 << bit0 << bit7 << bit8 <<  std::endl;///
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	return code;
}

uint8_t Type01Decoder::GetDistanceCode()
{
	// Distance code has 5 bits
	auto distBit4 = bitstream.GetNext();
	auto distBit3 = bitstream.GetNext();
	auto distBit2 = bitstream.GetNext();
	auto distBit1 = bitstream.GetNext();
	auto distBit0 = bitstream.GetNext();

	int distCode = (int)distBit0 | ((int)distBit1 << 1) | ((int)distBit2 << 2) | ((int)distBit3 << 3) | ((int)distBit4 << 4);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	std::cout << "distCode: " << std::setw(3) << std::setfill('0') << std::dec << (int)distCode << " " << distBit4 << distBit3 << distBit2 << distBit1 << distBit0 << std::endl;/////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Invalid distance code check
	if ((distCode == 30) || (distCode == 31))
	{
		throw RuntimeException("Invalid distance code " + std::to_string(distCode));
	}
	return static_cast<uint8_t>(distCode);
}

uint16_t Type01Decoder::GetLength(uint16_t code)
{
	// Get number of extra bits
	uint8_t extraLookup[6] = { (uint8_t)264, (uint8_t)268, (uint8_t)272, (uint8_t)276, (uint8_t)280, (uint8_t)284};
	uint8_t numberOfExtraBits = 0;
	for (uint8_t i = 0; i < 6; i++)
	{
		if (code <= extraLookup[i])
		{
			numberOfExtraBits = i;
			break;
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////
	std::cout << "code: " << (int)code << ", extra: " << (int)numberOfExtraBits << "   ";//////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////

	// Get Extra value
	uint8_t extraValue = 0;
	for (int8_t i = numberOfExtraBits - 1 ; i >= 0 ; i--)
	{
		uint8_t extraBit = bitstream.GetNext();
		extraValue = extraValue | (extraBit << i);
		///////////////////////////////////////////////////////////////////////////////////////////////////
		std::cout << extraBit;/////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////////////

	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	std::cout << std::endl << "extra value: " << (int)extraValue << std::endl;/////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////

	// Get Length
	// Codes 257 - 264 --> lengths 3 - 10
	if (code >= 257 && code <= 264) 
	{
		return code - 257 + 3;
	}

	// TODO refactor
	// Get start range value for code
	uint16_t startValue = 0;
	// TODO change to map or something
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
		startValue = 258;
		break;
	default:
		throw RuntimeException("Invalid Length Code " + std::to_string(code));
	}
	return startValue + extraValue;
}

uint16_t Type01Decoder::GetDistance()
{
	auto distanceCode = GetDistanceCode();
	if (distanceCode <= 3)
	{
		return distanceCode + 1;
	}

	uint16_t extra = 0;
	if (distanceCode > 3)
	{
		auto c = (distanceCode - 2) / 2;
		for (auto k = 0; k < c; k++)
		{
			int a = bitstream.GetNext();
			extra = extra | (a << k);
		}
		std::cout << "Distance code " << (int) distanceCode << ", extra: " << (int) c << ", extra value: " << extra << std::endl;
	}
		
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


/*
00011000 11101010
1 10 

00110001

10101111
10101111
10101111
111111111
00110000
00000100000011110110101001100010101001111111110110111101111000111001100111111111001100000101001011011000101111100111111111111111111111110010001100001111111110011001011001011011100000000110100001100001101101101111101000100110100110000001100000000010010000110000010010001000011000011011100010100001100000001111111111101101011111001100100110111111111100110000000001000111101111111111111111111111111111111111110000000000000000011110001111010000010001010001

*/