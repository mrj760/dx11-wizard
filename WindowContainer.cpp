#include "WindowContainer.h"

LRESULT WindowContainer::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//OutputDebugStringA("WINDOW PROC FROM WINDOW CONTAINER\n");
	switch (uMsg)
	{
	case WM_KEYDOWN:
	{
		unsigned char key = static_cast<unsigned char>(wParam);
		if (keyboard.isAutoRepeatKeys())
		{
			keyboard.onKeyPress(key);
		}
		else
		{
			const bool wasPressed = lParam & (1 << 30); // bit 30 of lParam contains what the last char pressed was
			if (!wasPressed)
			{
				keyboard.onKeyPress(key);
			}
		}
		return 0;
	}
	case WM_KEYUP:
	{
		unsigned char key = static_cast<unsigned char>(wParam);
		keyboard.onKeyRelease(key);
		return 0;
	}
	case WM_CHAR:
	{
		unsigned char ch = static_cast<unsigned char>(wParam);
		if (keyboard.isAutoRepeatChars())
		{
			keyboard.onCharPress(ch);
		}
		else
		{
			const bool wasPressed = lParam & (1 << 30); // bit 30 of lParam contains what the last char pressed was
			if (!wasPressed)
			{
				keyboard.onCharPress(ch);
			}
		}
		return 0;
	}
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}
