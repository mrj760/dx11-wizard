#pragma once
#include <comdef.h>
#include "StringConverter.h"

// macro which only take hrsult and message. (__FILE__, __FUNTION__, and __LINE__ are macros of their own)
#define COM_ERROR_IF_FAILED( hr, msg ) if( FAILED( hr ) ) throw COMException( hr, msg, __FILE__, __FUNCTION__, __LINE__ )

class COMException
{
public:
	// Take in: HandleResult, message, file in which error occured, function in which error occured, and line at which it occured
	COMException(HRESULT hr, const std::string& msg, const std::string& file, const std::string& function, int line)
	{
		_com_error error(hr);
		emsg = L"Msg: " + StringConverter::StringToWide(std::string(msg)) + L"\n";
		emsg += error.ErrorMessage();
		emsg += L"\nFile: " + StringConverter::StringToWide(file) + L"\n";
		emsg += L"Function: " + StringConverter::StringToWide(function) + L"\n";
		emsg += L"Line: " + StringConverter::StringToWide(std::to_string(line));
	}

	const wchar_t* what() const
	{
		return emsg.c_str();
	}

private:
	std::wstring emsg;
};

