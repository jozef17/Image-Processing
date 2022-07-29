#include "CommandLineParser.hpp"

#include <string>
#include <iostream>

std::map<std::string, std::vector<std::string>> CommandLineParser::GetArguments(int argc, char* argv[]) noexcept
{
	std::map<std::string, std::vector<std::string>> options;

	std::string option = "";
	for (int i = 1; i < argc; i++)
	{
		std::string val = argv[i];
		if (val.at(0) == '-')
		{
			std::vector<std::string> arg;
			options[val] = arg;
			option = val;
		}
		else
		{
			options[option].push_back(val);
		}
	}

	return options;
}