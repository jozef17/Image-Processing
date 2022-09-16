#include "HandlerManager.hpp"
#include "PrintMessageHandler.hpp"
#include "CompressionHandler.hpp"
#include "MessageConst.hpp"
#include "png/PngImage.hpp"
#include "BitmapImage.hpp"
#include "RAWImage.hpp"
#include "Utils.hpp"
#include "Exception.hpp"

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
		auto& parameters = options.find("-f") != options.end() ? options.at("-f") : options.at("--file");
		std::unique_ptr<Image> image = GetImage(parameters);

		float quality = 99.99f;
		if (options.find("-q") != options.end() ||
			options.find("--quality") != options.end())
		{
			auto& qparam = options.find("-q") != options.end() ? options.at("-q") : options.at("--quality");
			quality = GetQuality(qparam);
		}

		return std::unique_ptr<ArgumentHandler>(new CompressionHandler(std::move(image), quality));
	}

	return std::unique_ptr<ArgumentHandler>(new PrintMessageHandler(INVALID_INPUT_MESSAGE));
}

float HandlerManager::GetQuality(const std::vector<std::string> &arg)
{
	if (!Utils::IsNumber(arg.at(0)))
	{
		throw RuntimeException("Quality is not a number!");
	}

	auto quality = std::stof(arg.at(0));
	if (quality <= 0 || quality >= 100)
	{
		throw RuntimeException("Invalid Quality \"" + arg.at(0) + "\"!");
	}
	return quality;
}

std::unique_ptr<Image> HandlerManager::GetImage(const std::vector<std::string> &arg)
{
	if (arg.size() == 1)
	{
		auto lowercaseFile = Utils::ToLowercase(arg.at(0));
		if (Utils::EndsWith(lowercaseFile, ".png"))
		{
			return std::unique_ptr<Image>(new PngImage(arg.at(0)));
		}
		else if (Utils::EndsWith(lowercaseFile, ".bmp"))
		{
			return std::unique_ptr<Image>(new BitmapImage(arg.at(0)));
		}

		throw RuntimeException("Unsupported Image format file: \"" + arg.at(0) + "\"! Only png, bmp and raw images are supported!");		
	}
	else if (arg.size() == 3)
	{
		auto width = std::stoi(arg.at(1));
		auto height = std::stoi(arg.at(2));
		return std::unique_ptr<Image>(new RAWImage(arg.at(0), width, height));
	}

	throw RuntimeException("Invalid number of arguments!");
}
