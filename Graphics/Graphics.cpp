#include "Graphics.h"

bool Graphics::initialize(HWND hwnd, int width, int height)
{
	if (!initializeDirectX(hwnd, width, height))
	{
		return false;
	}
	return true;
}

void Graphics::RenderFrame()
{
	// Determine what background color will be
	float bgcolor[] = { 0,0,1.0f,1.0f }; // (RGBa: 0,0,1,1) : Blue
	
	// clear the render target
	this->deviceContext->ClearRenderTargetView(this->renderTargetView.Get(), bgcolor);

	// present it
	this->swapChain->Present(1 /*Vsync on (1 sync interval)*/, NULL/*Null flags*/);
}

bool Graphics::initializeDirectX(HWND hwnd, int width, int height)
{
	// need pointer to video adapter when creating a device. We don't want the default adapter, since it's not the one we always want.
	// We're going to assume we need the adapter with the highest memory is the best one for the job (Hopefully that means the user's GPU)
	std::vector<AdapterData> adapters = AdapterReader::getAdapters();

	if (adapters.size() < 1)
	{
		ErrorLogger::Log("No DXGI Adapters found.");
		return false;
	}

	DXGI_SWAP_CHAIN_DESC scd; /* Contains width, height, refresh rate, format, scanline ordering, and scaling */
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
	scd.BufferDesc.Width = width;
	scd.BufferDesc.Height = height;
	scd.BufferDesc.RefreshRate.Numerator = 60;	// if vsync turned off (or on but in windowed mode), refresh rate does nothing
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; /* 
														A four-component, 32-bit unsigned-normalized-integer format that supports 
														8 bits per channel including alpha.
														"(8 bits for R, G, B, and a)" */
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scd.SampleDesc.Count = 1; // Count 1: Number of multisamples per pixel
	scd.SampleDesc.Quality = 0; // Quality 0: image quality level (higher = lower performance)			
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // Use the surface or resource as an output render target.
	scd.BufferCount = 1; // 1: Double-buffer, 2: Triple-Buffer
	scd.OutputWindow = hwnd; // handle to window
	scd.Windowed = true; // set to windowed mode by default
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // discard frames after swapping out
												/* DXGI_SWAP_EFFECT_DISCARD (Value: 0)
												Use this flag to specify the bit-block transfer (bitblt) model and to specify 
												that DXGI discard the contents of the back buffer after you call IDXGISwapChain1::Present1.
												
												This flag is valid for a swap chain with more than one back buffer, although, 
												applications only have read and write access to buffer 0.
												Use this flag to enable the display driver to select the most efficient 
												presentation technique for the swap chain.

												Direct3D 12: This enumeration value is never supported. 
												D3D12 apps must using DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL or DXGI_SWAP_EFFECT_FLIP_DISCARD.*/
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; /* DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH:
														Set this flag to enable an application to switch modes by calling 
														IDXGISwapChain::ResizeTarget.
														When switching from windowed to full-screen mode, 
														the display mode (or monitor resolution) will be changed to match 
														the dimensions of the application window.*/



	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		adapters[0].pAdapter, // First IDXGI Adapter
		D3D_DRIVER_TYPE_UNKNOWN, // Tell DirectX to be ready for anything (unspecified)
		NULL, // no software driver
		NULL, // no runtime layers
		NULL, // no feature levels
		0, // # of feature levels
		D3D11_SDK_VERSION, //
		&scd, // swap chain description
		this->swapChain.GetAddressOf(), // address to swap chain
		this->device.GetAddressOf(), // address to device
		NULL, // no supported feature level
		this->deviceContext.GetAddressOf() // pointer to our device's context
		);

	if (FAILED(hr))
	{
		ErrorLogger::Log("Failed to create d3d11 device and swapchain");
	}

	// pointer to back buffer (get from swap chain)
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	// pass buffer slot, id of 2 texture, and address of where to store back buffer
	hr = this->swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "GetBuffer Failed.");
		return false;
	}

	// create render target view
	hr = this->device->CreateRenderTargetView(backBuffer.Get(), NULL, this->renderTargetView.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create render target view.");
		return false;
	}

	// set render target
	// Pass # of render targets, pointer to render target views, and pointer to depth stencil view
	this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), NULL);

	return true;
}
