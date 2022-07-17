#pragma once

#ifndef COMMAND_LINE_ARGUMENT_HPP__
#define COMMAND_LINE_ARGUMENT_HPP__

#include <string>
#include <vector>

class CommandLineOption final
{
public:
	CommandLineOption(std::string option) noexcept;

	void SetParameter(const std::string &param) noexcept;

	inline const std::string &GetOption() const noexcept { return this->option; };
	inline const std::vector<std::string> &GetParameters() const noexcept { return this->parameters; };

private:
	std::string option;
	std::vector<std::string> parameters;

};

#endif /* COMMAND_LINE_ARGUMENT_HPP__ */