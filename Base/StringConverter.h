#pragma once
#include <string>

class StringConverter
{
public:
	// convert normal strings to wide-strings, since that's what the D
	static std::wstring StringToWide(std::string str);
};