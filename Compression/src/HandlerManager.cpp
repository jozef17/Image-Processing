#include "HandlerManager.hpp"
#include "PrintMessageHandler.hpp"
#include "CompressionHandler.hpp"
#include "MessageConst.hpp"
#include "BitmapImage.hpp"
#include "RAWImage.hpp"

std::unique_ptr<ArgumentHandler> HandlerManager::GetHandler(const std::map<std::string, std::vector<std::string>>& options)
{
	if (options.find("-h") != options.end() ||
		options.find("--help") != options.end())
	{
		return std::unique_ptr<ArgumentHandler>(new PrintMessageHandler(HELP_MESSAGE));
	}

	if (options.find("-f") != options.end() ||
		options.find("--file") != options.end())
	{
		float quality = 100;
		std::unique_ptr<Image> image;
		
		const auto& parameters = options.find("-f") != options.end() ? options.at("-f") : options.at("--file");
		if (parameters.size() == 1)
		{
			image = std::unique_ptr<Image>(new BitmapImage(parameters.at(0)));
		}
		else if (parameters.size() == 3)
		{
			auto width = std::stoi(parameters.at(1));
			auto height = std::stoi(parameters.at(2));
			image = std::unique_ptr<Image>(new RAWImage(parameters.at(0), width, height));
		}

		if (options.find("-q") != options.end() ||
			options.find("--quality") != options.end())
		{
			// TODO get quality
		}

		return std::unique_ptr<ArgumentHandler>(new CompressionHandler(std::move(image), quality));
	}

	return std::unique_ptr<ArgumentHandler>(new PrintMessageHandler(INVALID_INPUT_MESSAGE));
}