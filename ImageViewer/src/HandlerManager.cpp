#include "HandlerManager.hpp"
#include "HelpHandler.hpp"
#include "InvalidInputHandler.hpp"
#include "ImageHandler.hpp"

std::unique_ptr<ArgumentHandler> HandlerManager::GetHandler(const std::map<std::string, std::vector<std::string>>& options)
{
	if (options.find("-h") != options.end() ||
		options.find("--help") != options.end())
	{
		return std::unique_ptr<ArgumentHandler>(new HelpHandler());
	}

	if (options.find("-f") != options.end() ||
		options.find("--file") != options.end())
	{
		const auto& parameters = options.find("-f") != options.end() ? options.at("-f") : options.at("--file");
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

	return std::unique_ptr<ArgumentHandler>(new InvalidInputHandler());
}