#include "CommandLineOption.hpp"

CommandLineOption::CommandLineOption(std::string option) noexcept  : option(option)
{}

void CommandLineOption::SetParameter(const std::string& param) noexcept
{
	this->parameters.push_back(param);
}
