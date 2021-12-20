#include "WindowContainer.h"

// Processes messages sent to the window
LRESULT WindowContainer::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//OutputDebugStringA("WINDOW PROC FROM WINDOW CONTAINER\n");
	switch (uMsg)
	{
		// Keyboard Messages
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
	// END Keyboard Messages

	//Mouse Messages
	case WM_MOUSEMOVE:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		mouse.moved(x, y);
		return 0;
	}
	case WM_LBUTTONDOWN:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		mouse.leftPressed(x, y);
		return 0;
	}
	case WM_RBUTTONDOWN:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		mouse.rightPressed(x, y);
		return 0;
	}
	case WM_MBUTTONDOWN:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		mouse.middlePressed(x, y);
		return 0;
	}
	case WM_LBUTTONUP:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		mouse.leftReleased(x, y);
		return 0;
	}
	case WM_RBUTTONUP:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		mouse.rightReleased(x, y);
		return 0;
	}
	case WM_MBUTTONUP:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		mouse.middleReleased(x, y);
		return 0;
	}
	case WM_MOUSEWHEEL:
	{
		int x = LOWORD(lParam);
		int y = HIWORD(lParam);
		
		int wheeldata = GET_WHEEL_DELTA_WPARAM(wParam);

		if (wheeldata > 0) mouse.wheelUp(x, y);
		else if (wheeldata < 0) mouse.wheelDown(x, y);
		else break;

		return 0;
	}
	// END Mouse Messages

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}
