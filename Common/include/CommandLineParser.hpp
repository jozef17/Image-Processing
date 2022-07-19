#pragma once

#ifndef COMMAND_LINE_PARSER_HPP__
#define COMMAND_LINE_PARSER_HPP__

#include <vector>

class CommandLineOption;

class CommandLineParser final
{
public:
	CommandLineParser() = delete;
	~CommandLineParser() = delete;

	static std::vector<CommandLineOption> GetArguments(int argc, char* argv[]) noexcept;

};

#endif /* COMMAND_LINE_PARSER_HPP__ */