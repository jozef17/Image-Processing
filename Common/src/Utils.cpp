#include "Utils.hpp"

#include<algorithm>

bool Utils::EndsWith(const std::string& str, const std::string& ending) noexcept
{
	auto strLen    = str.length();
	auto endingLen = ending.length();
	if (strLen < endingLen)
	{
		return false;
	}

	for (unsigned int i = 0; i < ending.length(); i++)
	{
		if (str.at(strLen - endingLen + i) != ending.at(i))
		{
			return false;
		}
	}

	return true;
}

bool Utils::IsNumber(const std::string& str) noexcept
{
	for (auto character : str)
	{
		if (!(character >= '0' && character <= '9' || character == '.'))
		{
			return false;
		}
	}

	return true;
}

std::string Utils::ToLowercase(std::string str) noexcept
{
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	return str;
}

std::string Utils::ToUppercase(std::string str) noexcept
{
	std::transform(str.begin(), str.end(), str.begin(), ::toupper);
	return str;
}
