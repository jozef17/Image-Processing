#include "CommandLineParser.hpp"
#include "CommandLineOption.hpp"

#include <string>
#include <iostream>

std::vector<CommandLineOption> CommandLineParser::GetArguments(int argc, char* argv[]) noexcept
{
	std::vector<CommandLineOption> options;

	CommandLineOption *arg = nullptr;
	for (int i = 1; i < argc; i++)
	{
		std::string val = argv[i];
		if (val.at(0) == '-')
		{
			CommandLineOption cmdArg = { val };
			options.push_back(cmdArg);
			arg = &options.back();
		}
		else
		{
			arg->SetParameter(val);
		}
	}

	return options;
}