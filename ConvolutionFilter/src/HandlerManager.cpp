#include "HandlerManager.hpp"
#include "PrintMessageHandler.hpp"
#include "MessageConst.hpp"
#include "Kernel.hpp"

std::unique_ptr<ArgumentHandler> HandlerManager::GetHandler(const std::map<std::string, std::vector<std::string>>& options)
{
	if (options.find("-h") != options.end() ||
		options.find("--help") != options.end())
	{
		return std::unique_ptr<ArgumentHandler>(new PrintMessageHandler(HELP_MESSAGE));
	}

	if (options.find("-p") != options.end() ||
		options.find("--print") != options.end())
	{
		auto kernel = Kernel::GetKernel("IDENTITY");
		if (options.find("-k") != options.end() && options.at("-k").size() > 0)
		{
			kernel = Kernel::GetKernel(options.at("-k").at(0));
		}
		if (options.find("--kernel") != options.end() && options.at("--kernel").size() > 0)
		{
			kernel = Kernel::GetKernel(options.at("--kernel").at(0));
		}

		return std::unique_ptr<ArgumentHandler>(new PrintMessageHandler(kernel.ToString()));
	}

	if (options.find("-f") != options.end() ||
		options.find("--file") != options.end())
	{
		auto kernel = Kernel::GetKernel("IDENTITY");
		if (options.find("-k") != options.end() && options.at("-k").size() > 0)
		{
			kernel = Kernel::GetKernel(options.at("-k").at(0));
		}
		if (options.find("--kernel") != options.end() && options.at("--kernel").size() > 0)
		{
			kernel = Kernel::GetKernel(options.at("--kernel").at(0));
		}

		// TODO handle convolution
	}

	return std::unique_ptr<ArgumentHandler>(new PrintMessageHandler(INVALID_INPUT_MESSAGE));
}