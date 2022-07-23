#include "HandlerManager.hpp"
#include "PrintMessageHandler.hpp"
#include "MessageConst.hpp"

std::unique_ptr<ArgumentHandler> HandlerManager::GetHandler(const std::map<std::string, std::vector<std::string>>& options)
{
	if (options.find("-h") != options.end() ||
		options.find("--help") != options.end())
	{
		return std::unique_ptr<ArgumentHandler>(new PrintMessageHandler(HELP_MESSAGE));
	}


	// TODO handle print kernel

	// Todo handle convolution

	return std::unique_ptr<ArgumentHandler>(new PrintMessageHandler(INVALID_INPUT_MESSAGE));
}