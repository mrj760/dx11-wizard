#pragma once
#include "ErrorLogger.h"

class WindowContainer; // forward decl to avoid ciruclar includes

class RenderWindow
{
public:
	// Initialized the window by taking: (Application instance, title, windows' class name, x size, y size)
	bool Initialize(WindowContainer* pWindowContainer, HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height);
	// Dispatches "messages" (e.g. you press a key or move your mouse) to a function which will handle the messages
	// If this returns false, the window will be closed.
	// If this returns true, keep going through the ProcessMessage loop
	bool ProcessMessages();
	~RenderWindow();
private:
	void RegisterWindowClass();
	// Store the handle to our window. Handle received upon creation of the window. (if received NULL, then the window failed to create)
	HWND handle = NULL;
	// Store the handle to the instance. 
	HINSTANCE hInstance = NULL;
	std::string window_title = "";
	std::wstring window_title_wide = L"";
	std::string window_class = "";
	std::wstring window_class_wide = L"";
	int width = 0;
	int height = 0;
};

