#include "ErrorLogger.h"
#include <comdef.h> // for _com_error data type

void ErrorLogger::Log(std::string message)
{
	std::string error_message = "Error: " + message;
	
	// Produce an error window using MessageBoxA (A: uses ASCII)
	MessageBoxA(NULL, error_message.c_str(), "Error", MB_ICONERROR);	// (Parent window, message text, title, message box type)
}

void ErrorLogger::Log(HRESULT hr, std::string message)
{
	//gets error information from the HRESULT about the API function that was called
	_com_error error(hr);
	
	// Wide-strings require an L before them		// error.ErrorMessage() used to obtain the error information form the HRESULT
	std::wstring error_message = L"Error: " + StringConverter::StringToWide(message) + L"\n" + error.ErrorMessage();
	// Produce an error window using MessageBoxW (W: uses Wide-String)
	MessageBoxW(NULL, error_message.c_str(), L"Error", MB_ICONERROR);	// (Parent window, message text, title, message box type)
}