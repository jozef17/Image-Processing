#pragma once

#ifndef PRINT_MESSAGE_HANDLER_HPP__
#define PRINT_MESSAGE_HANDLER_HPP__

#include "ArgumentHandler.hpp"
#include <string>

class PrintMessageHandler : public ArgumentHandler
{
public: 
	PrintMessageHandler(std::string message) noexcept;

	virtual int Run() override;

private:
	std::string message;

};

#endif /* PRINT_MESSAGE_HANDLER_HPP__ */