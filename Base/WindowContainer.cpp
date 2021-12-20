#include "WindowContainer.h"
#include <memory>

WindowContainer::WindowContainer()
{
	// Create Raw Input Device
	static bool raw_input_initialized = false; // only want to do this once so static
	if (!raw_input_initialized)
	{
		RAWINPUTDEVICE rid;

		rid.usUsagePage = 0x01; // mouse
		rid.usUsage = 0x02;	
		rid.dwFlags = 0; // default flags
		rid.hwndTarget = NULL; // window handle follows keyboard focus
		
		// Try to register the raw input device with the above data
		if (!RegisterRawInputDevices(&rid, 1, sizeof(rid)))
		{
			ErrorLogger::Log(GetLastError(), "Failed to Reigster Raw Input Devices.");
			exit(-1);
		}

		raw_input_initialized = true;
	}
}

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
	// Take in position data and call appropriae MouseClass functions
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
	case WM_INPUT:
	{
		UINT dataSize;

		// get the data size of the raw input from lParam
		GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, NULL, &dataSize, sizeof(RAWINPUTHEADER));

		// don't wanna do things if there's no data
		if (dataSize > 0)
		{
			// point to the raw data
			std::unique_ptr<BYTE[]> rawdata = std::make_unique<BYTE[]>(dataSize);
			// see if the input is equal to the raw data we are expecting
			if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, rawdata.get(), &dataSize, sizeof(RAWINPUTHEADER)) == dataSize);
			{
				// send the raw input data to our mouse class
				RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(rawdata.get());
				if (raw->header.dwType == RIM_TYPEMOUSE)
				{
					mouse.rawMove(raw->data.mouse.lLastX, raw->data.mouse.lLastY);
				}
			}
		}

		return DefWindowProc(hwnd, uMsg, wParam, lParam); // system must perform necessary clean-up
	}
	// END Mouse Messages

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}
