#include "AdapterReader.h"

std::vector<AdapterData> AdapterReader::adapters; // just defining static member

AdapterData::AdapterData(IDXGIAdapter* pAdapter)
{
    this->pAdapter = pAdapter;
    HRESULT hr = pAdapter->GetDesc(&this->description);
    if (FAILED(hr))
    {
        ErrorLogger::Log(hr, "Failed to get Description for IDXGIAdapter.");
    }
}

std::vector<AdapterData> AdapterReader::getAdapters()
{
    if (adapters.size() > 0) // already initialized
        return adapters;

    // Create DXGIFactory object (weird shit)
    Microsoft::WRL::ComPtr<IDXGIFactory> pFactory;
    HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(pFactory.GetAddressOf()));
    if (FAILED(hr))
    {
        ErrorLogger::Log(hr, "Failed to create DXGIFactory for enumerating adapters");
        exit(-1);
    }

    IDXGIAdapter* pAdapter;
    UINT index=0;
    // populate a pointer with an adapter and add to vector of adapters
    while (SUCCEEDED(pFactory->EnumAdapters(index, &pAdapter)))
    {
        adapters.push_back(AdapterData(pAdapter));
        index += 1;
    }
    return adapters;
}
