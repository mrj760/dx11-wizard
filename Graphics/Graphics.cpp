#include "Graphics.h"


bool Graphics::initialize(HWND hwnd, int width, int height)
{
	timer.startTimer();

	this->width = width;
	this->height = height;
	if (!initializeDirectX(hwnd))
		return false;

	if (!initializeShaders())
		return false;

	return true;
}

void Graphics::renderFrame()
{
	/* TIMER */
	static int fpsCount = 0;
	static std::string fpsStr = "FPS: 0";
	fpsCount += 1;
	if (timer.elapsedMS() > 1000.0)
	{
		fpsStr = "FPS: " + std::to_string(fpsCount);
		fpsCount = 0;
		timer.restartTimer();
	}

	/* BACKGROUND */

	// Determine what background color will be
	float bgcolor[] = { 0.1f, 0.1f, 0.1f, 1.0f }; // Grey background

	/* CLEAR FRAME */
	
	// clear the render target (clear the window) and clear the depth stencil
	this->deviceContext->ClearRenderTargetView(
		this->renderTargetView.Get(), 
		bgcolor);
	this->deviceContext->ClearDepthStencilView(
		this->depthStencilView.Get(), 
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, // Clear both depth and stencil
		1.0f, // set depth to 1
		0); // initialize stencil to 0

	/* INPUT LAYOUT */

	// set input layout
	this->deviceContext-> 
		IASetInputLayout(this->vertexshader.getInputLayout());
	
	/* TOPOLOGY */

	// set topology
	this->deviceContext-> 
		IASetPrimitiveTopology(
			D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	/* RASTERIZER */
	// set rasterizer state
	this->deviceContext->
		RSSetState(this->rasterizerState.Get());

	/* DEPTH STENCIL */
	// set depth stencil state
	this->deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);

	/* SAMPLER */

	// Set sampler state
	this->deviceContext->PSSetSamplers(0, 1, this->samplerState.GetAddressOf());

	/* SHADERS */

	// Set shaders
	this->deviceContext-> // set vertex shader
		VSSetShader(vertexshader.getShader(), NULL, 0);
	this->deviceContext-> // set pixel shader
		PSSetShader(pixelshader.getShader(), NULL, 0);
	// set Shader Resources
	this->deviceContext->PSSetShaderResources(0, 1, myTexture.GetAddressOf()); // shaders for our context
	
	/* CAMERA */

	dx::XMMATRIX worldOrigin = dx::XMMatrixIdentity(); // world origin

	// move cam up and down
	/*f3 campos = cam.getpositionFloat3();
	static bool down = true, right = true;
	if (campos.y < -1.5f || campos.y > 1.5f)
		down = !down;
	if (campos.x < -1.5f || campos.x > 1.5f)
		right = !right;
	cam.adjustPosition(
		right ? 0.01f : -0.01f, 
		down ? 0.01f : -0.01f, 
		0.0f);
	cam.setTargetPos(f3(0.f, 0.f, 0.f));*/

	/* BUFFERS */

	// Update Constant Buffer
	constantBuffer.data.mat = worldOrigin * cam.getViewMatrix() * cam.getProjectionMatrix(); // all vertices set to (world origin* view * projection)
	constantBuffer.data.mat = dx::XMMatrixTranspose(constantBuffer.data.mat); // turn it from column_major to row_major format
	if (!constantBuffer.ApplyChanges())
		return;
	deviceContext-> // apply the changes to the constant buffer
		VSSetConstantBuffers(0, 1, constantBuffer.getAddressOf());

	// Update Vertex Buffer
	UINT offset = 0;	// at which slot to begin when reading in the vertex buffer data
	this->deviceContext->IASetVertexBuffers( // vertices for our context
		0,								// start slot
		1,								// for now: only one buffer 
		vertexBuffer.getAddressOf(),	// vertex buffer to use
		vertexBuffer.stridePtr(),		// how big of a data size to iterate over
		&offset);						// at which slot to begin when reading in the vertex buffer data

	// Update Index Buffer
	this->deviceContext->IASetIndexBuffer( // indices for our context
		indicesBuffer.get(), // indeces buffer to use
		DXGI_FORMAT_R32_UINT, // reading 32 bit uints
		0); // no offset

	/* DRAWING */
	
	// Draw Index Buffer data
	this->deviceContext->DrawIndexed(
		indicesBuffer.getBufferSize(), // # of vertices to draw
		0, // initial vertex index
		0); // base vertex index

	// Draw Text "Hello World" and the FPS at top left
	spriteBatch->Begin();
	std::wstring str = L"Hello World\nFPS: " + StringConverter::StringToWide(fpsStr);
	spriteFont->DrawString(
		spriteBatch.get(), 
		str.c_str(),
		dx::XMFLOAT2(0, 5), // position
		dx::Colors::Bisque, // color
		0,					// rotation
		dx::XMFLOAT2(0, 0), // origin
		dx::XMFLOAT2(1,1)	// scale
		);					// default effects and layer depth parameters
	spriteBatch->End();

	/* PRESENTING */

	// present our frame
	this->swapChain->Present(
		0,		// Vsync (1: On [1 sync interval] , 0: off) 
		NULL);	//Null flags
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

	/* We are inputting these shader variables to our shaders*/
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		// Pixel position
		{
			"POSITION",
			0 /* specify 0 in case 2 semantics have the same name
				(useful in instancing when passing matrices to input layout) */,
		DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, /* Format of the data (3 32-bit floats) */
			0, /* Input Slot */
			0, /* Offset */ // Can also use Macro Magic instead of 0: D3D11_APPEND_ALIGNED_ELEMENT
			D3D11_INPUT_PER_VERTEX_DATA, /* Input Slot Class (Either vertex data or instance data) */
			0 /* Instance data step rate: # of instances to draw using same per-instance data before advancing in the buffer by one element */
		},

		// Pixel Color
		{
			"TEXCOORD",
			0, /* Semantics */
			DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, /* Using 2 floats */
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

bool Graphics::initializeDirectX(HWND hwnd)
{
	/* ADAPTERS */

	// initialize adapters
	/* Need pointer to video adapter when creating a device. 
		We don't want the default adapter, since that's not always the one we want. */
	std::vector<AdapterData> adapters = AdapterReader::getAdapters(); // Assume adapter with highest memory best (Hopefully that means the GPU)
	// fatal if less than one video adapter
	if (adapters.size() < 1)
	{
		ErrorLogger::Log("No DXGI Adapters found.");
		return false;
	}

	/* SWAPCHAIN */

	// Create description of swapchain
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
		// DXGI_SWAP_EFFECT_DISCARD (Value: 0) Explanation :
			/* Use this flag to specify the bit - block transfer(bitblt) model and to specify
			that DXGI discard the contents of the back buffer after you call IDXGISwapChain1::Present1.

			This flag is valid for a swap chain with more than one back buffer, although,
			applications only have read and write access to buffer 0.
			Use this flag to enable the display driver to select the most efficient
			presentation technique for the swap chain.

			Direct3D 12: This enumeration value is never supported.
			D3D12 apps must using DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL or DXGI_SWAP_EFFECT_FLIP_DISCARD.*/
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	// Explanation of DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH:
		/* Set this flag to enable an application to switch modes by calling
		IDXGISwapChain::ResizeTarget.
		When switching from windowed to full-screen mode,
		the display mode (or monitor resolution) will be changed to match
		the dimensions of the application window.*/
	
	// Create device and swapchain
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
		this->deviceContext.GetAddressOf()); // pointer to our device's context

	if (FAILED(hr))
	{
		ErrorLogger::Log("Failed to create d3d11 device and swapchain");
		return false;
	}

	// populate a back buffer using the swap chain
	mwrl::ComPtr<ID3D11Texture2D> backBuffer;
	hr = this->swapChain->GetBuffer(
		0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));

	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "GetBuffer Failed.");
		return false;
	}

	/* RENDER TARGET VIEW */

	// create render target view
	hr = this->device->CreateRenderTargetView(
		backBuffer.Get(), NULL, this->renderTargetView.GetAddressOf());

	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create render target view.");
		return false;
	}

	/* STENCIL */

	// Create description of depth stencil buffer
	D3D11_TEXTURE2D_DESC depthStencilTexDesc;
	depthStencilTexDesc.Width = width;
	depthStencilTexDesc.Height = height;
	depthStencilTexDesc.MipLevels = 1;	// 
	depthStencilTexDesc.ArraySize = 1;	// 
	depthStencilTexDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// 32 bit z-buffer format. 24 bits for depth, 8 bits for stencil
	depthStencilTexDesc.SampleDesc.Count = 1;
	depthStencilTexDesc.SampleDesc.Quality = 0;
	depthStencilTexDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilTexDesc.CPUAccessFlags = 0;	// don't need access to cpu
	depthStencilTexDesc.MiscFlags = 0;

	// Create texture / Depth stencil buffer
	hr = this->device->CreateTexture2D(
		&depthStencilTexDesc, NULL /* No initial sub resource data */, 
		this->depthStencilBuffer.GetAddressOf() /* Where to store buffer */);

	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create depth stencil buffer");
		return false;
	}

	// Create depth stencil view
	hr = this->device->CreateDepthStencilView(
		this->depthStencilBuffer.Get(), NULL, this->depthStencilView.GetAddressOf());

	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create depth stencil view");
		return false;
	}

	// set stencil's render target
	this->deviceContext->OMSetRenderTargets(
		1, /* # of render targets */ 
		this->renderTargetView.GetAddressOf(), /* pointer to render target views */ 
		this->depthStencilView.Get() /* pointer to depth stencil view */
		);

	// Create stencil state
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	depthStencilDesc.DepthEnable = true; // enable depth
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL; // turn on stencil
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL; // how to decide whether to replace pixel based on z-depth
	hr = this->device->CreateDepthStencilState(
		&depthStencilDesc, this->depthStencilState.GetAddressOf());

	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create depth stencil state");
		return false;
	}

	/* VIEWPORT */

	// Create the viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.f;
	viewport.MaxDepth = 1.f;

	// set the viewport
	this->deviceContext->RSSetViewports(1/*1 viewport*/, &viewport);

	/* RASTERIZER */

	// Create rasterizer state
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID; // solid fill (as opposed to wireframe)
	rasterizerDesc.CullMode= D3D11_CULL_MODE::D3D11_CULL_BACK; // don't draw back-facing triangles 
																// (clockwise pixels : front-facing, counter-clockwise : back-facing)
	hr = this->device->CreateRasterizerState(
		&rasterizerDesc, this->rasterizerState.GetAddressOf());

	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create rasterizer state");
		return false;
	}

	/* FONTS */

	// Init fonts
	spriteBatch = std::make_unique<dx::SpriteBatch>(this->deviceContext.Get());
	spriteFont = std::make_unique<dx::SpriteFont>(this->device.Get(), L"Data/Fonts/lucida_console_16.spritefont");

	/* SAMPLER */

	// init sampler state
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = this->device->CreateSamplerState(
		&sampDesc, this->samplerState.GetAddressOf()); //Create sampler state

	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create sampler state.");
		return false;
	}

	return true;
}

bool Graphics::initializeScene()
{	
	// Textured Square
	Vertex v[] =
	{
		Vertex(-0.5f,  -0.5f, 0.0f, 0.0f, 1.0f), //Bottom Left - [0]
		Vertex(-0.5f,   0.5f, 0.0f, 0.0f, 0.0f), //Top Left - [1]
		Vertex(0.5f,   0.5f, 0.0f, 1.0f, 0.0f), //Top Right - [2]
		Vertex(0.5f,  -0.5f, 0.0f, 1.0f, 1.0f), //Bottom Right - [3]
		//Vertex(-0.5f, -0.5f, 1.0f, 0.0f, 1.0f), //Bottom Left  // --repeat
	};

	/* VERTEX BUFFER */

	// Create vertex buffer
	HRESULT hr = this->vertexBuffer.initialize(this->device.Get(), v, ARRAYSIZE(v));

	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create vertex buffer");
		return false;
	}

	/* INDECES BUFFER (of vertices) */

	// Create Indeces Array
	DWORD indices[]
	{
		0,1,2,
		0,2,3,
	};// using indeces fixes reusing multiple instances of the same vertex data when drawing pixels
	
	// Create description for indeces buffer
	hr = indicesBuffer.initialize(device.Get(), indices, ARRAYSIZE(indices));

	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create indices buffer.");
		return false;
	}

	/* TEXTURE LOADING */

	// Load a texture
	hr = dx::CreateWICTextureFromFile(
		this->device.Get(), L"Data/Textures/boi.png", nullptr, myTexture.GetAddressOf());

	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create wic texture from file");
		return false;
	}

	/* CONSTANT BUFFER(S) */
	hr = constantBuffer.initialize(device.Get(), deviceContext.Get());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to init constant buffer");
		return false;
	}

	cam.setPosition(0.0f, 0.0f, -2.0f);
	cam.setProjectionValues(90.0f /*FOV*/, (float)width / (float)height /*Aspect Ratio*/, 0.1f/*near Z*/, 1000.0f/*far Z*/);

	return true;
}
