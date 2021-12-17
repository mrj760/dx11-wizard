#include "RenderWindow.h"

bool RenderWindow::Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height)
{
	this->hInstance = hInstance;
	this->window_title = window_title;
	this->window_title_wide = StringConverter::StringToWide(this->window_title);
	this->window_class = window_class;
	this->window_class_wide = StringConverter::StringToWide(this->window_class);
	this->width = width;
	this->height = height;

	this->RegisterWindowClass();

	// Class is registered. Ready to Create our window.
	this->handle = CreateWindowEx(	// Extended window class. 
		0, // Default Extended Windows Style. See : https://msdn.microsoft.com/en-us/library/windows/desktop/ff700543(v=vs.85).aspx
		this->window_class_wide.c_str(),	// window class name
		this->window_title_wide.c_str(),	// window title name
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,	// Normal Windows style. See : https://msdn.microsoft.com/en-us/library/windows/desktop/ms632600(v=vs.85).aspx
		0,	// window X Position
		0,	// window Y Position
		this->width,
		this->height,
		NULL,	// Handle to parent window. This is first window so no parent.
		NULL,	// Handle to menu or child window id. Can be set to NULL and use menu in WindowClassEx
		this->hInstance,	// Handle to the instance of module to be used with this window
		nullptr);	// paramt to create window

	// Make sure the window was successfully created
	if (this->handle == NULL)
	{
		ErrorLogger::Log(GetLastError(), "CreateWindowEx Failed for window" + this->window_title);
		// have to call GetLastError() since CreateWindowEx returns a handle instead of a HRESULT
		return false;
	}

	// Bring the window on screen and set it as foreground and main focus
	ShowWindow(this->handle, SW_SHOW);
	SetForegroundWindow(this->handle);
	SetFocus(this->handle);

	return true;
}

bool RenderWindow::ProcessMessages()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	// read in the message
	if (PeekMessage(&msg, this->handle, 0, 0, PM_REMOVE)) // See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms644943(v=vs.85).aspx
		// PeekMessage rather than GetMessage because Peek does not lock
		// takes : ( address of the message (where to store it), the handle to the window we're checking messages for, 
		//			min filter msg value, max filter msg value (we'ere not filtering), 
		//			how to handle the message : remove message after capturing it via PeekMessage (see: https://msdn.microsoft.com/en-us/library/windows/desktop/ms644943(v=vs.85).aspx)
	{
		TranslateMessage(&msg); // Translates key messages to char messages (See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms644955(v=vs.85).aspx)
		DispatchMessage(&msg);	// Send the message to default window's processing funciton. (See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms644934(v=vs.85).aspx)
	}

	// check if window was closed after reading a message
	if (msg.message == WM_NULL) // Not WM_QUIT so that we can account for more than one window
	{
		// since message might be null despite the window not being closed...
		// check if the handle is a window
		if (!IsWindow(this->handle))
		{
			// window was destroyed. make handle null and unregister class.
			// Destruciton is called automatically.
			this->handle = NULL;
			UnregisterClass(this->window_class_wide.c_str(), this->hInstance);
			return false;
		}
	}

	return true;
}

RenderWindow::~RenderWindow()
{
	if (this->handle != NULL)
	{
		UnregisterClass(this->window_class_wide.c_str(), this->hInstance);
		DestroyWindow(handle);
	}
}

void RenderWindow::RegisterWindowClass()
{
	WNDCLASSEX wc;	// EX(Extended version of window class) Must be filled before window can be created. See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms633577(v=vs.85).aspx
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; //Flags [Redraw on width/height change from resize/movement]. See: https://msdn.microsoft.com/en-us/library/windows/desktop/ff729176(v=vs.85).aspx
	wc.lpfnWndProc = DefWindowProc;	// Pointer to Window Processing function (for handling messages from this window)
	wc.cbClsExtra = 0;	// # of extra bytes to allocate following the window-class structure
	wc.cbWndExtra = 0;	// # of extra bytes to allocate following the window instance
	wc.hInstance = this->hInstance;	// handle to our instance
	wc.hIcon = NULL;	// window icon
	wc.hIconSm = NULL;	// small version of icon
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); // Default cursor
	wc.hbrBackground = NULL;	// Handle to the class background brush for window's background color. For stock brushes, see: https://msdn.microsoft.com/en-us/library/windows/desktop/dd144925(v=vs.85).aspx
	wc.lpszMenuName = NULL;		// Pointer to null terminated string for the menu
	wc.lpszClassName = this->window_class_wide.c_str();	// Pointer to null terminated string of our class name for the window
	wc.cbSize = sizeof(WNDCLASSEX);	// Need to fill in size of our struct for cbSize
	RegisterClassEx(&wc); // Must register the class so it becomes usable
}
