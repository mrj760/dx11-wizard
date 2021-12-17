#include "RenderWindow.h"
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "DirectXTK.lib")

int APIENTRY wWinMain(
	_In_ HINSTANCE	hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR		lpCmdLine,
	_In_ int		nCmdShow)
{
	/* Testing Error Logging */
	//ErrorLogger::Log(S_OK, "Task Failed Successfully"); // tells "The operation completed successfully"
	//ErrorLogger::Log(E_INVALIDARG, "Test Message"); // tells "The parameter was incorrect"
	//HRESULT hr = E_INVALIDARG;
	//if (FAILED(hr))
	//{
	//	ErrorLogger::Log(hr, "Test Failure"); // Logs the error of an invalid HRESULT
	//}
	//HRESULT hr = S_OK;
	//if (SUCCEEDED(hr))
	//{
	//	MessageBoxA(NULL, "Success", "Success", NULL); // Just produces a message box saying success
	//	//ErrorLogger::Log(hr, "Test Success"); // Logs the error of a successful HRESULT
	//}

	// Create and initialize window
	RenderWindow rw;
	rw.Initialize(hInstance, "Title", "MyWindowClass", 800, 600);
	while (rw.ProcessMessages())	// keep going until our message processor says the window has been closed
	{
		Sleep(50);
	}


	
	return 0;
}