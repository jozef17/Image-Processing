#include <gtest/gtest.h>
#include "CommandLineParser.hpp"
#include "CommandLineOption.hpp"


TEST(CommandLineParserTest, NoOption)
{
	char* argv[1] = { "exe" };
	auto result = CommandLineParser::GetArguments(1,argv);

	EXPECT_EQ(result.size(), 0);
}

TEST(CommandLineParserTest, OneOption)
{
	char* argv[2] = { "exe", "--help"};
	auto result = CommandLineParser::GetArguments(2, argv);

	EXPECT_EQ(result.size(), 1);
	EXPECT_EQ(result.at(0).GetOption(), "--help");
}

TEST(CommandLineParserTest, MultipleOption)
{
	char* argv[4] = { "exe", "--version", "--directory", "./"};
	auto result = CommandLineParser::GetArguments(4, argv);

	EXPECT_EQ(result.size(), 2);
	EXPECT_EQ(result.at(0).GetOption(), "--version");
	EXPECT_EQ(result.at(1).GetOption(), "--directory");
	EXPECT_EQ(result.at(0).GetParameters().size(), 0);
	EXPECT_EQ(result.at(1).GetParameters().size(), 1);
	EXPECT_EQ(result.at(1).GetParameters().at(0), "./");
}
