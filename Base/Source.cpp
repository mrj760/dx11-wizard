#include "Engine.h"

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
	Engine engine;
	engine.Initialize(hInstance, "Title", "MyWindowClass", 800, 600);
	while (engine.ProcessMessages())	// keep processing messages from our render window until our message processor says the window has been closed
	{
		engine.Update();
		engine.RenderFrame();
	}


	
	return 0;
}

// INPUT ASSEMBLER: Must setup - DONE
// VERTEX SHADER:	Must program - DONE

// RASTERIZER:		Must setup
// PIXEL SHADER:	Must program
// OUTPUT MERGER:	Must setup