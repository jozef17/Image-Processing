#pragma once

#ifndef COMMAND_LINE_PARSER_HPP__
#define COMMAND_LINE_PARSER_HPP__

#include <map>
#include <string>
#include <vector>

class CommandLineOption;

class CommandLineParser final
{
public:
	CommandLineParser() = delete;
	~CommandLineParser() = delete;

	static std::map<std::string,std::vector<std::string>> GetArguments(int argc, char* argv[]) noexcept;

};

#endif /* COMMAND_LINE_PARSER_HPP__ */