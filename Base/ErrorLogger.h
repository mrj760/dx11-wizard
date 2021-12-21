#pragma once
#include "StringConverter.h"
#include <Windows.h>
class ErrorLogger
{
public:
	/*Two types of Error Logging methods:
	1) By simply taking in a string message
	2) Taking in a data type used when checking if Windows/DirectX function calls were successful (and a string message) */
	static void Log(std::string message);
	static void Log(HRESULT hr, std::string message);
	static void Log(HRESULT hr, std::wstring message);
};