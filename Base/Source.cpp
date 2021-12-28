#include "Engine.h"

int APIENTRY wWinMain(
	_In_ HINSTANCE	hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR		lpCmdLine,
	_In_ int		nCmdShow)
{
	// Create and initialize window
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "CoInitialize failed.");
		exit(-1);
	}
	Engine engine;
	if (engine.Initialize(hInstance, "Title", "MyWindowClass", 800, 600));
	{
		while (engine.ProcessMessages()) // keep processing messages from our render window until our message processor says the window has been closed
		{
			engine.Update();
			engine.RenderFrame();
		}
	}
	/* END OF MAIN LOOP */


	engine.Shutdown();


	
	return 0;
}