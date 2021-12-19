#pragma once
#include "RenderWindow.h"
#include "Keyboard/KeyboardClass.h"

// This class will be inherited by other classes that have windows tied to them
class WindowContainer
{
public:
	LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:
	RenderWindow render_window;
	KeyboardClass keyboard;
};

