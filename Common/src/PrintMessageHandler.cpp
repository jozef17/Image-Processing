#include "PrintMessageHandler.hpp"
#include <iostream>

PrintMessageHandler::PrintMessageHandler(std::string message) : message(message)
{}

int PrintMessageHandler::Run()
{
	std::cout << this->message << std::endl;
	return 0;
}
