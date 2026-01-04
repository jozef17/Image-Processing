#include <gtest/gtest.h>
#include <vector>
#include <cstring>

#include "jpg/EntropyDecoder.hpp"
#include "HuffmanCode.hpp"
#include "BitStream.hpp"
#include "Exception.hpp"

TEST(EntropyDecoderTest, DecodeError)
{
	// Setup inputs
	std::vector<HuffmanCode> codes = {{ 0, 5, 4}};

	uint8_t bits[] = {0xff};
	std::unique_ptr<uint8_t[]> data(new uint8_t[sizeof(bits)]);
	std::memcpy(data.get(), bits, sizeof(bits));

	BitStream bitStream(BitStream::Mode::MSB);
	bitStream.Append(std::move(data), sizeof(bits));

	// Execute decoder
	EntropyDecoder decoder(bitStream);
	EXPECT_THROW(decoder.DecodeBlock(codes, codes), RuntimeException);
}

/// Test data were sourced from real image (10x10 pixels white) cut to single 8x8 block
TEST(EntropyDecoderTest, DecodeWhite)
{
	// Setup inputs
	std::vector<HuffmanCode> dcCodes = {{9, 63, 7}};
	std::vector<HuffmanCode> acCodes = {{ 0, 5, 4}};

	uint8_t bits[] = { 0xfd, 0xfc, 0xa2};
	std::unique_ptr<uint8_t[]> data(new uint8_t[sizeof(bits)]);
	std::memcpy(data.get(), bits, sizeof(bits));

	BitStream bitStream(BitStream::Mode::MSB);
	bitStream.Append(std::move(data), sizeof(bits));

	// Execute decoder
	EntropyDecoder decoder(bitStream);
	auto block = decoder.DecodeBlock(dcCodes, acCodes);

	// Check decoded values
	EXPECT_EQ(block[0], 508);
	for (int i = 1; i < 64; i++)
	{
		EXPECT_EQ(block[i], 0);
	}
}
