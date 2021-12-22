#include "Graphics.h"
#include <string>

bool Graphics::initialize(HWND hwnd, int width, int height)
{
	if (!initializeDirectX(hwnd, width, height))
		return false;

	if (!initializeShaders())
		return false;

	return true;
}

void Graphics::renderFrame()
{
	// Determine what background color will be
	float bgcolor[] = { 0.1f, 0.1f, 0.1f, 1.0f }; // Grey background
	
	// clear the render target
	this->deviceContext->ClearRenderTargetView(this->renderTargetView.Get(), bgcolor);

	// necessary before drawing...
	this->deviceContext-> // set input layout
		IASetInputLayout(this->vertexshader.getInputLayout());
	this->deviceContext-> // set topology
		IASetPrimitiveTopology(
			D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	this->deviceContext->
		RSSetState(this->rasterizerState.Get());

	this->deviceContext-> // set vertex shader
		VSSetShader(vertexshader.getShader(), NULL, 0);
	this->deviceContext-> // set pixel shader
		PSSetShader(pixelshader.getShader(), NULL, 0);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	this->deviceContext->IASetVertexBuffers(0, 1 /* for now: only one buffer*/, vertexBuffer.GetAddressOf(), &stride, &offset);


	// draw
	this->deviceContext->Draw(3 /* # of vertices to draw */, 0 /* vertex offset */);

	// present it
	this->swapChain->Present(1 /*Vsync on (1 sync interval)*/, NULL/*Null flags*/);
}

bool Graphics::initializeShaders()
{

	std::wstring shaderFolder = L"";
	#pragma region DetermineShaderPath

	// Account for different environments
	if (IsDebuggerPresent())
	{
		#ifdef _DEBUG // Debug mode
			#ifdef _WIN64 // x64
				shaderFolder = L"x64\\Debug\\";
			#else	// x86 (Win32)
				shaderFolder = L"Debug\\";
		#endif

		#else	// Release Mode
			#ifdef _WIN64 //x64
				shaderFolder = L"x64\\Release\\";
			#else	// x86 (Win32)
				shaderFolder = L"Release\\";
			#endif
		#endif
	}

	/* We are inputting these things to directx */
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		// Pixel position
		{
			"POSITION",
			0 /* specify 0 in case 2 semantics have the same name
				(useful in instancing when passing matrices to input layout) */,
			DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, /* Format of the data (2 32-bit floats)
										(if we were using 3 32-bit floats we add B32 after G32)
										likewise, can add A32 after B32 */
			0, /* Input Slot */
			0, /* Offset */ // Can also use Macro Magic instead of 0: D3D11_APPEND_ALIGNED_ELEMENT
			D3D11_INPUT_PER_VERTEX_DATA, /* Input Slot Class (Either vertex data or instance data) */
			0 /* Instance data step rate: # of instances to draw using same per-instance data before advancing in the buffer by one element */
		},

		// Pixel Color
		{
			"COLOR",
			0, /* Semantics */
			DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, /* Using 3 floats for color (rgb) */
			0, /* Input slot */
			D3D11_APPEND_ALIGNED_ELEMENT, /* Macro for offset (?) */
			D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, /* Input slot class */
			0 /* Instance data step rate */
		}
	};

	UINT numElements = ARRAYSIZE(layout);

	// Init vertex shader
	if (!vertexshader.initialize(this->device, shaderFolder + L"vertexshader.cso", layout, numElements))
	{
		return false;
	}
	
	// init pixel shader
	if (!pixelshader.initialize(this->device, shaderFolder + L"pixelshader.cso"))
	{
		return false;
	}

	// init the scene
	if (!initializeScene())
	{
		return false;
	}

	return true;
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

	// Create the viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = width;
	viewport.Height = height;

	// set the viewport
	this->deviceContext->RSSetViewports(1/*1 viewport*/, &viewport);

	// Create rasterizer state
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	// solid fill (as opposed to wireframe)
	rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	// don't draw back-facing triangles (clockwise pixels are front-facing, counter-clockwise is back-facing)
	rasterizerDesc.CullMode= D3D11_CULL_MODE::D3D11_CULL_BACK;

	hr = this->device->CreateRasterizerState(&rasterizerDesc, this->rasterizerState.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create rasterizer state");
		return false;
	}


	return true;
}

bool Graphics::initializeScene()
{	
	// Vertices must be listed in clockwise order
	Vertex v[] =
	{
		Vertex(-.5f, -0.5f, 1.0f, 0.f, 0.f), // red bottom left
		Vertex(0.f, 0.5f, 0.f, 1.f, 0.f), // blue top center
		Vertex(0.5f, -0.5f, 0.f, 0.f, 1.f), // green bottom right
	};

	// Create description for vertex buffer
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * ARRAYSIZE(v);
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;
	ZeroMemory(&vertexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	vertexBufferData.pSysMem = v;

	HRESULT hr = this->device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, this->vertexBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create vertex buffer");
		return false;
	}

	return true;
}
