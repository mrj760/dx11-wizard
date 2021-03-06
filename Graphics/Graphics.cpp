#include "Graphics.h"
namespace imgui = ImGui;


bool Graphics::initialize(HWND hwnd, int width, int height)
{
	// begin timer
	timer.startTimer();

	// setup window dimensions
	this->width = width;
	this->height = height;
	
	// setup directX with handle to window
	if (!initializeDirectX(hwnd))
		return false;

	// setup various shaders
	if (!initializeShaders())
		return false;

	// Setup ImGUI
	IMGUI_CHECKVERSION();	// 
	imgui::CreateContext();	// 
	ImGuiIO& io = imgui::GetIO();	// 
	ImGui_ImplWin32_Init(hwnd);	// Needs handle to window for Windows
	ImGui_ImplDX11_Init(device.Get(), deviceContext.Get());	// Needs device and context for DirectX
	imgui::StyleColorsDark();	// nice


	return true;
}

void Graphics::renderFrame()
{
	/* FPS TIMER */
	static int fpsCount = 0;
	static std::string fpsStr = "FPS: 0";
	fpsCount += 1;
	if (timer.elapsedMS() > 1000.0)
	{
		fpsStr = "FPS: " + std::to_string(fpsCount);
		fpsCount = 0;
		timer.restartTimer();
	}

	/* GUI */
	// Start ImGui frame for directX and windows // See :https://github.com/ocornut/imgui/blob/master/imgui.cpp
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	imgui::NewFrame();
	imgui::Begin("Test");

	/* DEVICE CONTEXT */

	/* BACKGROUND */
	float bgcolor[] = { 0.1f, 0.1f, 0.1f, 1.0f }; // Grey background
	/* CLEAR FRAME */
	this->deviceContext->ClearRenderTargetView( this->renderTargetView.Get(), bgcolor);
	this->deviceContext->ClearDepthStencilView( this->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, /* Clear both depth and stencil */ 1.0f, /* set depth to 1 */ 0); /* initialize stencil to 0 (?) */
	/* INPUT LAYOUT */
	this->deviceContext->IASetInputLayout(this->vertexshader.getInputLayout());
	/* TOPOLOGY */
	this->deviceContext-> IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	/* DEPTH STENCIL */
	this->deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);
	/* SAMPLER */
	this->deviceContext->PSSetSamplers(0, 1, this->samplerState.GetAddressOf());
	/* SHADERS */
	this->deviceContext-> VSSetShader(vertexshader.getShader(), NULL, 0); // set vertex shader
	this->deviceContext-> PSSetShader(pixelshader.getShader(), NULL, 0); // set pixel shader
	/* BLEND STATE */
	deviceContext->OMSetBlendState(NULL, NULL /*Not using blend factor*/, 0xFFFFFFFF);

	/* DRAWING */

	/* Blue */
	{
		model.draw(cam.getViewMatrix() * cam.getProjectionMatrix());
	}

	// Draw Text "Hello World" and the FPS at top left
	spriteBatch->Begin();
	std::wstring str = L"Hello World\nFPS: " + StringConverter::StringToWide(fpsStr);
	// Position, color, rotation, origin, scale... default effects and layer depth parameters
	spriteFont->DrawString(spriteBatch.get(), str.c_str(), dx::XMFLOAT2(0, 5), dx::Colors::Bisque, 0, dx::XMFLOAT2(0, 0), dx::XMFLOAT2(1, 1));
	spriteBatch->End();

	/* CAMERA */

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

	/* IM GUI */

	// imgui finished
	imgui::End();
	imgui::Render();
	ImGui_ImplDX11_RenderDrawData(imgui::GetDrawData()); // draw the gui with directx

	/* PRESENTING */

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
			0 /* specify 0 in case two semantics have the same name (useful in instancing when passing matrices to input layout) */,
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
		return false;
	
	// init pixel shader
	if (!pixelshader.initialize(this->device, shaderFolder + L"pixelshader.cso"))
		return false;

	// init the scene
	if (!initializeScene())
		return false;

	return true;
}

bool Graphics::initializeDirectX(HWND hwnd)
{
	try
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
		DXGI_SWAP_CHAIN_DESC scd = { 0 }; /* Contains width, height, refresh rate, format, scanline ordering, and scaling */
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
		COM_ERROR_IF_FAILED(hr, "Failed to create d3d11 device and swapchain");

		// populate a back buffer using the swap chain
		mwrl::ComPtr<ID3D11Texture2D> backBuffer;
		hr = this->swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
		COM_ERROR_IF_FAILED(hr, "GetBuffer failed");

		/* RENDER TARGET VIEW */

		// create render target view
		hr = this->device->CreateRenderTargetView(backBuffer.Get(), NULL, this->renderTargetView.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create render target view");

		/* STENCIL */

		// Create description of depth stencil buffer
		CD3D11_TEXTURE2D_DESC depthStencilTextureDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, width, height, 1, 1, D3D11_BIND_DEPTH_STENCIL);
		D3D11_TEXTURE2D_DESC depthStencilTexDesc;

		// Create texture / Depth stencil buffer
		hr = this->device->CreateTexture2D(
			&depthStencilTextureDesc, NULL /* No initial sub resource data */,
			this->depthStencilBuffer.GetAddressOf() /* Where to store buffer */);
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil buffer");

		// Create depth stencil view
		hr = this->device->CreateDepthStencilView(this->depthStencilBuffer.Get(), NULL, this->depthStencilView.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil view");

		// set stencil's render target (# of render targets, pointer to render target views, pointer to depth stencil view)
		this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get());

		// Create stencil state
		CD3D11_DEPTH_STENCIL_DESC depthStencilDesc(D3D11_DEFAULT);
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL; // how to decide whether to replace pixel based on z-depth
		hr = this->device->CreateDepthStencilState(&depthStencilDesc, this->depthStencilState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil state");

		/* VIEWPORT */

		// Create the viewport
		CD3D11_VIEWPORT viewport(0.0f, 0.0f, (float)width, (float)height);

		// set the viewport
		this->deviceContext->RSSetViewports(1/*1 viewport*/, &viewport);

		/* RASTERIZER */

		// Rasterizer for back-culling
		CD3D11_RASTERIZER_DESC rasterizerDescBack(D3D11_DEFAULT);
		hr = this->device->CreateRasterizerState(&rasterizerDescBack, this->rasterizerBack.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create rasterizer state");

		// Rasterizer for front-culling
		CD3D11_RASTERIZER_DESC rasterizerDescFront(D3D11_DEFAULT);
		rasterizerDescFront.CullMode = D3D11_CULL_FRONT;
		hr = this->device->CreateRasterizerState(&rasterizerDescFront, this->rasterizerFront.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create rasterizer state");

		/* BLENDING */

		// Create Blend State
		D3D11_BLEND_DESC blendDesc = { 0 };

		D3D11_RENDER_TARGET_BLEND_DESC rtbd = { 0 }; // Render target blend view

		rtbd.BlendEnable = true;	// enable blending

		// (Final color) = [(Source pixel) * (Source Blend Factor)] + [(Destination pixel) * (Destination Blend Factor)]
		// Multiply the source pixel color by its alpha, and add it to the destination color multiplied by the inverse of the source alpha
		// ex: source:(0, 1, 0, .9), dest:(1,0,0,1) ... final color is (0, (1 * 0.9), 0) + ((1 * (1-.9)), 0, 0) = (0,.9,0) + (.1,0,0) = (0.1, 0.9, 0)
		rtbd.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;			// base the source color on the source alpha
		rtbd.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;	// base the destination alpha on the source alpha
		rtbd.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;			// add the two colors together

		// (Final alpha) = [(Source alpha) * (Source blend factor)] + [(Destination alpha) * (Destination blend factor)]
		// With values set below...	(Final alpha) =	(Source alpha)
		rtbd.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;			// multiply source-blend-alpha by 1 (same thing as it already was)
		rtbd.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;		// Do not take into account (looking at equation above, this value being set to 0 makes the dest. blend alpha meaningless)
		rtbd.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;		// add the two alphas together
		rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;	// take into account red, green, blue, and alpha levels (all)

		blendDesc.RenderTarget[0] = rtbd; // only have one render target
		hr = device->CreateBlendState(&blendDesc, blendState.GetAddressOf()); // Create the blend state from the description
		COM_ERROR_IF_FAILED(hr, "Failed to create blend state");

		/* FONTS */

		// Init fonts
		spriteBatch = std::make_unique<dx::SpriteBatch>(this->deviceContext.Get());
		spriteFont = std::make_unique<dx::SpriteFont>(this->device.Get(), L"Data/Fonts/proggy_clean_16.spritefont");

		/* SAMPLER */

		// init sampler state
		CD3D11_SAMPLER_DESC sampDesc(D3D11_DEFAULT);
		ZeroMemory(&sampDesc, sizeof(sampDesc));
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		hr = this->device->CreateSamplerState(&sampDesc, this->samplerState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create sampler state");
	}
	catch (COMException e)
	{
		ErrorLogger::Log(e);
		return false;
	}

	return true;
}

bool Graphics::initializeScene()
{	
	try
	{
		/* TEXTURE LOADING */

		// Load face texture
		HRESULT hr = dx::CreateWICTextureFromFile(this->device.Get(), L"Data/Textures/boi.png", nullptr, face.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create texture from file");

		// Load blue texture
		hr = dx::CreateWICTextureFromFile(this->device.Get(), L"Data/Textures/blue.png", nullptr, blue.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file");

		// Load bird texture
		hr = dx::CreateWICTextureFromFile(this->device.Get(), L"Data/Textures/bird.png", nullptr, bird.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file");

		// Load water texture
		hr = dx::CreateWICTextureFromFile(this->device.Get(), L"Data/Textures/water.png", nullptr, water.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file");

		/* CONSTANT BUFFER(S) */
		hr = cb_vs_vertexshader.initialize(device.Get(), deviceContext.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to init constant buffer");

		hr = cb_ps_pixelshader.initialize(device.Get(), deviceContext.Get());
		COM_ERROR_IF_FAILED(hr, "Failed to init constant buffer");

		/* MODELS */
		if (!model.initialize(device.Get(), deviceContext.Get(), blue.Get(), cb_vs_vertexshader))
			return false;

		model.setPosition(0.f, 0.f, 2.f);
		cam.setPosition(0.0f, 0.0f, 0.0f);
		cam.setProjectionValues(90.0f /*FOV*/, (float)width / (float)height /*Aspect Ratio*/, 0.1f/*near Z*/, 1000.0f/*far Z*/);

	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);
		return false;
	}
	return true;
}
