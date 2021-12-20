#pragma once
#include "../Base/ErrorLogger.h""
#include <d3d11.h>
#include <wrl/client.h> // allows ComPtr which basically work like smart pointers for Com objects
#include <vector>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "DirectXTK.lib")
#pragma comment(lib, "DXGI.lib")

class AdapterData
{
public:
	// contains pointer to adapter and description
	AdapterData(IDXGIAdapter* pAdapter);
	IDXGIAdapter* pAdapter = nullptr;
	DXGI_ADAPTER_DESC description;
};

class AdapterReader
{
public:
	static std::vector<AdapterData> getAdapters();
private:
	static std::vector<AdapterData> adapters; // have all windows refer to the same adapaters
};