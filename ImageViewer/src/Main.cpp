#include "ArgumentHandler.hpp"
#include "CommandLineParser.hpp"
#include "Exception.hpp"
#include "HandlerManager.hpp"

#include <iostream>

int main(int argc, char* argv[])
{
	try 
	{
		auto options = CommandLineParser::GetArguments(argc, argv);
		auto handler = HandlerManager::GetHandler(options);
		return handler->Run();
	}
	catch (Exception& exception)
	{
		std::cerr << "Error: " << exception.what() << std::endl;
		return 1;
	}
	catch (std::exception& exception)
	{
		std::cerr << "Error: " << exception.what() << std::endl;
		return 1;
	}
}
