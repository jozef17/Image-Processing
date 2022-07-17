#include "HandlerManager.hpp"
#include "HelpHandler.hpp"
#include "InvalidInputHandler.hpp"
#include "ImageHandler.hpp"
#include "CommandLineOption.hpp"

std::unique_ptr<ArgumentHandler> HandlerManager::GetHandler(const std::vector<CommandLineOption>& options)
{
	for (const auto& option : options)
	{
		const auto& value = option.GetOption();
		if (value == "-h" || value == "--help")
		{
			return std::unique_ptr<ArgumentHandler>(new HelpHandler());
		}
		else if (value == "-f" || value == "--file")
		{
			const auto& parameters = option.GetParameters();
			if (parameters.size() == 1)
			{
				return std::unique_ptr<ArgumentHandler>(new ImageHandler(parameters.at(0)));
			}
			else if (parameters.size() == 3)
			{
				auto width = std::stoi(parameters.at(1));
				auto height = std::stoi(parameters.at(2));
				return std::unique_ptr<ArgumentHandler>(new ImageHandler(parameters.at(0), width, height));
			}
		}
	}

	return std::unique_ptr<ArgumentHandler>(new InvalidInputHandler());
}