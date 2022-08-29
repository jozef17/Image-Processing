#include "png/Type10Decoder.hpp"
#include "png/BitStream.hpp"
#include "png/Adler32.hpp"
#include "Exception.hpp"

#include <algorithm>

#include <iostream>

struct Code
{
	uint16_t value;
	uint16_t code;
	uint8_t length;

	/// Generates codes for lengths
	static void AsignCodes(std::vector<Code>& codes);

	/// Comparison for sorting 
	bool operator<(const Code& other) const;

private:
	static std::vector<uint16_t> GetCodes(uint8_t bit, std::vector<uint16_t> oldCodes);
};

void Code::AsignCodes(std::vector<Code>& codes)
{
	uint8_t minLength = std::min_element(codes.begin(), codes.end())->length;
	uint8_t maxLength = std::max_element(codes.begin(), codes.end())->length;

	// TODO check if uint16_t is enough 
	std::vector<uint16_t> values = { 0, 1 };
	for (uint8_t i = 1; i < minLength; i++)
	{
		values = GetCodes(i, values);
	}

	for (uint8_t i = minLength; i <= maxLength; i++)
	{
		for(auto &code : codes)
		{
			if (code.length != i)
			{
				continue;
			}

			code.code = values[0];
			values.erase(values.begin());
		}
		values = GetCodes(i, values);
	}

	for (auto a : codes)//////////////////////////////////////////////////////////
	{/////////////////////////////////////////////////////////////////////////////
		std::cout << std::dec << (int)a.value << " " << std::dec << (int)a.length << "  ";
		for (int bit = 0; bit < a.length; bit++)/////////////////////////////////////////
		{/////////////////////////////////////////////////////////////////////////
			bool b = (a.code & 1 << bit) >> bit;//////////////////////////////////
			std::cout << std::dec << (int)b;//////////////////////////////////////
		}/////////////////////////////////////////////////////////////////////////
		std::cout << std::endl;///////////////////////////////////////////////////
	}/////////////////////////////////////////////////////////////////////////////

}

std::vector<uint16_t> Code::GetCodes(uint8_t bit, std::vector<uint16_t> oldCodes)
{
	std::vector<uint16_t> ret;
	for (auto val : oldCodes)
	{
		ret.push_back(val);
		ret.push_back(val | 1 << bit);
	}
	return ret;
}

bool Code::operator<(const Code& other) const
{
	if (this->length == other.length)
	{
		return this->value < other.value;
	}
	return this->length < other.length;
}

Type10Decoder::Type10Decoder(BitStream& bitstream) : bitstream(bitstream)
{}

std::vector<uint8_t> Type10Decoder::Decode()
{
	std::vector<uint8_t> data;
	std::cout << "compressed with dynamic Huffman codes" << std::endl;

	// 5 Bits: HLIT, # of Literal / Length codes - 257 (257 - 286)
	// 5 Bits: HDIST, # of Distance codes - 1        (1 - 32)
	// 4 Bits: HCLEN, # of Code Length codes - 4     (4 - 19)
	/*////////////////////////////////////////////////////////////////////////////
	std::cout << std::dec << "HLIT : ";//////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////*/
	uint8_t hlit = Get(5);
	/////////////////////////////////////////////////////////////////////////////
	std::cout << " " << std::dec << (int)hlit << std::endl;//////////////////////
	std::cout << std::dec << "HDIST: ";//////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////*/
	uint8_t hdist = Get(5);
	/////////////////////////////////////////////////////////////////////////////
	std::cout << " " << std::dec << (int)hdist << std::endl;/////////////////////
	std::cout << std::dec << "HCLEN: ";//////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////*/
	uint8_t hclen = Get(4);
	/////////////////////////////////////////////////////////////////////////////
	std::cout << " " << std::dec << (int)hclen << std::endl;/////////////////////
	////////////////////////////////////////////////////////////////////////////*/

	// Get alphabets
	// Read and process code lengths
	auto codeLengths = GetCodeLengths(hclen);

	// Decode literal length alphabet
	// HLIT + 257 code lengths for the literal / length alphabet,
	//	encoded using the code length Huffman code
	auto llCodes = GetAlphabet(hlit + 257, codeLengths);
	////////////////////////////////////////////////////////////////////////////////////////////
	std::cout << "------------------------------------------------------------" << std::endl;///
	std::cout << "Literal/Length codes: " << std::endl;/////////////////////////////////////////
	for (auto& code : llCodes)	////////////////////////////////////////////////////////////////
	{	////////////////////////////////////////////////////////////////////////////////////////
		std::cout << " " << (int)code.value << " (" << (int)code.length << "):   ";/////////////
		for (int i = 0; i < code.length;i++)////////////////////////////////////////////////////
		{///////////////////////////////////////////////////////////////////////////////////////
			std::cout << (int)((code.code & 1 << i) >> i);//////////////////////////////////////
		}///////////////////////////////////////////////////////////////////////////////////////
		std::cout << std::endl;/////////////////////////////////////////////////////////////////
	}	////////////////////////////////////////////////////////////////////////////////////////
	std::cout << "------------------------------------------------------------" << std::endl;///
	////////////////////////////////////////////////////////////////////////////////////////////

	// Decode distance alphabet
	// HDIST + 1 code lengths for the distance alphabet, encoded using the code length Huffman code
	auto distCodes = GetAlphabet(hdist+1, codeLengths);
	////////////////////////////////////////////////////////////////////////////////////////////
	std::cout << "------------------------------------------------------------" << std::endl;///
	std::cout << "Distance codes: " << std::endl;/////////////////////////////////////////
	for (auto& code : distCodes)	////////////////////////////////////////////////////////////
	{	////////////////////////////////////////////////////////////////////////////////////////
		std::cout << " " << (int)code.value << " (" << (int)code.length << "):   ";/////////////
		for (int i = 0; i < code.length; i++)///////////////////////////////////////////////////
		{///////////////////////////////////////////////////////////////////////////////////////
			std::cout << (int)((code.code & 1 << i) >> i);//////////////////////////////////////
		}///////////////////////////////////////////////////////////////////////////////////////
		std::cout << std::endl;/////////////////////////////////////////////////////////////////
	}	////////////////////////////////////////////////////////////////////////////////////////
	std::cout << "------------------------------------------------------------" << std::endl;///
	////////////////////////////////////////////////////////////////////////////////////////////

	while(true)
	{
		// The actual compressed data of the block, encoded using the 
		// literal / length and distance Huffman codes
		auto code = Get(llCodes);
		////////////////////////////////////////////////////////////////////////////////////////////
		std::cout << " llCode: " << (int)code << std::endl;/////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////

		if (code < 256)
		{
			data.push_back(code);
		}
		else if (code == 256)
		{
			// The literal / length symbol 256 (end of data), encoded using the literal / length Huffman code
			break;
		}
		else
		{
			auto length = GetLength(code);


			auto distance = Get(distCodes);
//			auto length = (code + 3) - 256; //+ 3; // TODO from 3?
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			std::cout << "  length: " << std::dec << length << ", distance: " << (int)distance << std::endl;//
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			auto dist = GetDistance(distance);
			auto start = (int)data.size() - dist;
			for (int i = 0; i < length; i++)
			{
				if ((start + i) < 0)
				{
					throw RuntimeException("Invalid distance! Start position: \'" + std::to_string(start + i) + "\' for distance: " + std::to_string((int)distance));
				}
				else
				{
					auto value = data.at(start + i);
					//std::cout << "   Adding:" << (int)value << std::endl;
					data.push_back(value);
				}
			}
		}

		// ?????????????????????????????????????????????????????????????????
		// The code length repeat codes can cross from HLIT + 257 to the
		// HDIST + 1 code lengths.In other words, all code lengths form
		// a single sequence of HLIT + HDIST + 258 values.
		// ?????????????????????????????????????????????????????????????????
	}
	return data;
}

uint16_t Type10Decoder::Get(uint8_t numOfBits)
{
	uint16_t value = 0;
	for (uint8_t i = 0; i < numOfBits; i++)
	{
		auto bit = this->bitstream.GetNext();
//		value = value << 1 | bit;  // ?????
		value = value | bit << i;  // ?????
		////////////////////////////////////////////////////////////////////////////
		std::cout << (int)bit;//////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////
	}
	return value;
}

uint16_t Type10Decoder::Get(const std::vector<Code>& codes)
{
	uint8_t length = codes.at(0).length;
	auto value = Get(length);
	for (const auto& code : codes)
	{
		if (code.length > length)
		{
			for (length; length < code.length; length++)
			{
				auto bit = this->bitstream.GetNext();
//				value = value << 1 | bit;  // ?????
				value = value | bit << length;  // ?????
				////////////////////////////////////////////////////////////////////////////
				std::cout << (int)bit;//////////////////////////////////////////////////////
				////////////////////////////////////////////////////////////////////////////

			}
		}

		if (code.code == value)
		{
			return code.value;
		}
	}

	throw RuntimeException("Decoding Failed");
}

std::vector<Code> Type10Decoder::GetCodeLengths(uint8_t hclen)
{
	// (HCLEN + 4) x 3 bits: code lengths for the code length alphabet given just
	// above, in the order : 
	//           16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
	// These code lengths are interpreted as 3 - bit integers (0 - 7); as above, 
	// a code length of 0 means the corresponding symbol(literal / length or 
	// distance code length) is not used.
	uint8_t indexes[19] = { 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };

	// Read lengths
	std::vector<Code> codeLengths;
	for (uint8_t i = 0; i < hclen + 4; i++)
	{
		auto length = Get(3);
		////////////////////////////////////////////////////////////////////////////////////////////////
		std::cout << " ";///////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////
		if (length > 0)
		{
			Code code = { 0 };
			code.value = indexes[i];
			code.length = length;
			codeLengths.push_back(code);
		}
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	std::cout << std::endl;///////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	std::sort(codeLengths.begin(), codeLengths.end());

	// Asign codes
	Code::AsignCodes(codeLengths);

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	std::cout << "Code Lengths: ";/////////////////////////////////////////////////////////////////////////
	for (uint8_t i = 0; i < codeLengths.size(); i++)///////////////////////////////////////////////////////////////////////
	{//////////////////////////////////////////////////////////////////////////////////////////////////////
		std::cout << std::dec << "(" << (int)codeLengths[i].value << ")" << (int)codeLengths[i].length << " ";
	}//////////////////////////////////////////////////////////////////////////////////////////////////////
	std::cout << std::endl;////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	return codeLengths;
}

// decodes literal length codes
// TODO change to Decode alphabet ???
std::vector<Code> Type10Decoder::GetAlphabet(uint16_t numElements, const std::vector<Code>& distCodes)
{
	std::vector<Code> codes;

	while (codes.size() < numElements )
	{
		auto val = Get(distCodes);
		/////////////////////////////////////////////////////////////////////////////
		std::cout << "? " << std::dec << (int)val << std::endl;//////////////////////
		/////////////////////////////////////////////////////////////////////////////

		if (val < 16)
		{
			Code code = { 0 };
			code.length = val;
			code.value = codes.size();
			codes.push_back(code);
		}
		if (val == 16)
		{
			// 16: Copy the previous code length 3 - 6 times.
			// 	The next 2 bits indicate repeat length (0 = 3, ..., 3 = 6)
			// 	Example:  Codes 8, 16 (+2 bits 11),
			// 	16 (+2 bits 10) will expand to
			// 	12 code lengths of 8 (1 + 6 + 5)
			auto repeatLength = Get(2) + 3;
			/////////////////////////////////////////////////////////////////////////////////////
			std::cout << std::endl;
			std::cout << "(16) Adding " << std::dec << (int)(codes.at(codes.size() - 1).length) << " " << (int)repeatLength << " times" << std::endl;
			/////////////////////////////////////////////////////////////////////////////////////

			auto code = codes.at(codes.size() - 1);
			for (uint16_t i = 0; i < repeatLength; i++)
			{
				code.value = codes.size();
				codes.push_back(code);
			}
		}
		else if (val == 17)
		{
			// 17: Repeat a code length of 0 for 3 - 10 times. (3 bits of length)
			auto repeatLength = Get(3) + 3;
			/////////////////////////////////////////////////////////////////////////////////////
			std::cout << std::endl;
			std::cout << "(17) Adding " << std::dec << (int)repeatLength << " zeroes" << std::endl;
			/////////////////////////////////////////////////////////////////////////////////////
			Code code = { 0 };
			for (uint16_t i = 0; i < repeatLength; i++)
			{
				code.value = codes.size();
				codes.push_back(code);
			}
		}
		else if (val == 18)
		{
			auto repeatLength = Get(7) + 11;
			/////////////////////////////////////////////////////////////////////////////////////
			std::cout << std::endl;
			std::cout << "(18) Adding " << std::dec << (int)repeatLength << " zeroes" << std::endl;
			/////////////////////////////////////////////////////////////////////////////////////

			// 18: Repeat a code length of 0 for 11 - 138 times (7 bits of length)
			Code code = { 0 };
			for (uint16_t i = 0; i < repeatLength; i++)
			{
				code.value = codes.size();
				codes.push_back(code);
			}
		}
	}

	// Remove 0 lengths
	codes.erase(
		std::remove_if( 
			codes.begin(), 
			codes.end(), 
			[](const Code& code){return code.length == 0;}
		),
		codes.end()
	);

	std::sort(codes.begin(), codes.end());
	Code::AsignCodes(codes);

	return codes;
}

uint16_t Type10Decoder::GetLength(uint16_t code)
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
	///////////////////////////////////////////////////////////////////////////////////////////////////
	std::cout << "code: " << (int)code << ", extra " << (int)numberOfExtraBits << "bits   ";///////////
	//////////////////////////////////////////////////////////////////////////////////////////////////*/

	// Get Extra value
	uint8_t extraValue = 0;
	for (int8_t i = 0; i < numberOfExtraBits; i++)
	{
		uint8_t extraBit = bitstream.GetNext();
		extraValue = (extraBit << i) | extraValue;//(extraValue << 1) | extraBit;
		///////////////////////////////////////////////////////////////////////////////////////////////////
		std::cout << (int)extraBit;////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////*/

	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	std::cout << std::endl << "extra value: " << (int)extraValue << std::endl;/////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////*/

	// Get Length
	// Codes 257 - 264 --> lengths 3 - 10
	if (code >= 257 && code <= 264)
	{
		return (code - 257) + 3;
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
		return 258;
	default:
		throw RuntimeException("Invalid Length Code " + std::to_string(code));
	}
	return startValue + extraValue;
}

uint16_t Type10Decoder::GetDistance(uint32_t distanceCode)
{
	if (distanceCode <= 3)
	{
		return distanceCode + 1;
	}

	uint16_t extra = 0;
	auto c = (distanceCode - 2) / 2;
	for (auto k = 0; k < c; k++)
	{
		int bit = bitstream.GetNext();
		extra = (bit << k) | extra; // (extra << 1) | bit;
	}
	std::cout << "Distance code " << (int)distanceCode << ", extra bits: " << (int)c << ", extra value: " << extra << std::endl;

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
