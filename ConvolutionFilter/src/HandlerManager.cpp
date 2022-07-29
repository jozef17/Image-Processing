#include "HandlerManager.hpp"
#include "PrintMessageHandler.hpp"
#include "FilterHandler.hpp"
#include "MessageConst.hpp"
#include "Kernel.hpp"
#include "BitmapImage.hpp"
#include "RAWImage.hpp"

std::unique_ptr<ArgumentHandler> HandlerManager::GetHandler(const std::map<std::string, std::vector<std::string>>& options)
{
	// Help
	if (options.find("-h") != options.end() ||
		options.find("--help") != options.end())
	{
		return std::unique_ptr<ArgumentHandler>(new PrintMessageHandler(HELP_MESSAGE));
	}

	// Print kernel
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

	// Apply Convolution filter
	if (options.find("-f") != options.end() ||
		options.find("--file") != options.end())
	{
		// Get Kernel
		auto kernel = Kernel::GetKernel("IDENTITY");
		if (options.find("-k") != options.end() && options.at("-k").size() > 0)
		{
			kernel = Kernel::GetKernel(options.at("-k").at(0));
		}
		if (options.find("--kernel") != options.end() && options.at("--kernel").size() > 0)
		{
			kernel = Kernel::GetKernel(options.at("--kernel").at(0));
		}

		// Get image and return handler
		const auto& parameters = options.find("-f") != options.end() ? options.at("-f") : options.at("--file");

		std::shared_ptr<Image> image;
		if (parameters.size() == 1)
		{
			image = std::shared_ptr<Image>(new BitmapImage(parameters.at(0)));
		}
		else if (parameters.size() == 3)
		{
			auto width = std::stoi(parameters.at(1));
			auto height = std::stoi(parameters.at(2));
			image = std::shared_ptr<Image>(new RAWImage(parameters.at(0), width, height));
		}
		
		Filter filter = { image, std::move(kernel) };
		return std::unique_ptr<ArgumentHandler>(new FilterHandler(std::move(filter)));
	}

	return std::unique_ptr<ArgumentHandler>(new PrintMessageHandler(INVALID_INPUT_MESSAGE));
}