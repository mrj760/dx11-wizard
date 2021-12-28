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
	/* fPS TIMER */
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
	/* RASTERIZER */
	//this->deviceContext->RSSetState(this->rasterizerBack.Get());
	/* DEPTH STENCIL */
	this->deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);
	/* SAMPLER */
	this->deviceContext->PSSetSamplers(0, 1, this->samplerState.GetAddressOf());
	/* SHADERS */
	this->deviceContext-> VSSetShader(vertexshader.getShader(), NULL, 0); // set vertex shader
	this->deviceContext-> PSSetShader(pixelshader.getShader(), NULL, 0); // set pixel shader
	/* BLEND STATE */
	deviceContext->OMSetBlendState(blendState.Get(), NULL /*Not using blend factor*/, 0xFFFFFFFF);


	static float alpha = 0.3f;
	/* Blue */
	{
		static dx::XMVECTOR tloffset = dx::XMVECTOR{ 0,0,-1 };
		static dx::XMVECTOR scaling = dx::XMVECTOR{ 2,2,2 };
		// total = scaling * translation
		static dx::XMMATRIX total = dx::XMMatrixScalingFromVector(scaling) * dx::XMMatrixTranslationFromVector(tloffset);
		static UINT offset = 0;

		// Update Constant Vertex Buffer
		cb_vs_vertexshader.data.matx = total * cam.getViewMatrix() * cam.getProjectionMatrix(); // all vertices set to (world origin* view * projection)
		cb_vs_vertexshader.data.matx = dx::XMMatrixTranspose(cb_vs_vertexshader.data.matx); // turn it from column_major to row_major format
		if (!cb_vs_vertexshader.ApplyChanges()) return;
		deviceContext->VSSetConstantBuffers(0, 1, cb_vs_vertexshader.getAddressOf()); // apply the changes to the constant buffer

		// Update Constant Buffer Pixel Shader
		cb_ps_pixelshader.data.alpha = alpha;
		if (!cb_ps_pixelshader.ApplyChanges()) return;
		deviceContext->PSSetConstantBuffers(0, 1, cb_ps_pixelshader.getAddressOf());

		// Set texure
		this->deviceContext->PSSetShaderResources(0, 1, blue.GetAddressOf()); // shader resource (texture)
		// Update Vertex Buffer : (start slot, # of buffers, pointers to buffers to use, stride length, pointer to offset)
		this->deviceContext->IASetVertexBuffers(offset, 1, vertexBuffer.getAddressOf(), vertexBuffer.stridePtr(), &offset);
		// Update Index Buffer
		this->deviceContext->IASetIndexBuffer(indicesBuffer.get(), /*indices buffer to use*/ DXGI_FORMAT_R32_UINT, /*reading 32 bit uints*/ offset);

		// Rasterize and draw the front sizes then the back sides
		this->deviceContext->RSSetState(this->rasterizerFront.Get());
		this->deviceContext->DrawIndexed(indicesBuffer.getBufferSize(), 0, 0);
		this->deviceContext->RSSetState(this->rasterizerBack.Get());
		this->deviceContext->DrawIndexed(indicesBuffer.getBufferSize(), 0, 0);
	}

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

	

	// Draw Text "Hello World" and the FPS at top left
	spriteBatch->Begin();
	std::wstring str = L"Hello World\nFPS: " + StringConverter::StringToWide(fpsStr);
	// Position, color, rotation, origin, scale... default effects and layer depth parameters
	spriteFont->DrawString( spriteBatch.get(), str.c_str(), dx::XMFLOAT2(0, 5), dx::Colors::Bisque, 0, dx::XMFLOAT2(0, 0), dx::XMFLOAT2(1,1));
	spriteBatch->End();

	/* IM GUI */

	// ur gay button
	static int counter = 0;
	imgui::Text("oi boi");
	if (imgui::Button("oi boi cliq me eh"))
	{
		counter += 1;
	}
	imgui::SameLine();
	std::string haha = counter > 0 ? "ur gay" : "";
	if (counter > 1)
	{
		haha += 'x' + std::to_string(counter);
	}
	imgui::Text(haha.c_str());

	// slider to control alpha of texture
	imgui::DragFloat("Alpha", &alpha, 0.1f, 0.0f, 1.0f);

	// imgui finished
	imgui::End();
	imgui::Render();
	ImGui_ImplDX11_RenderDrawData(imgui::GetDrawData()); // draw the gui with directx

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

	// Create rasterizer state for back
	D3D11_RASTERIZER_DESC rasterizerDescBack;
	ZeroMemory(&rasterizerDescBack, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerDescBack.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID; // solid fill (as opposed to wireframe)
	rasterizerDescBack.CullMode= D3D11_CULL_MODE::D3D11_CULL_BACK; // don't draw back-facing triangles 
																// (clockwise pixels : front-facing, counter-clockwise : back-facing)
	hr = this->device->CreateRasterizerState(
		&rasterizerDescBack, this->rasterizerBack.GetAddressOf());

	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create rasterizer state");
		return false;
	}

	// Create rasterizer state for front culling
	D3D11_RASTERIZER_DESC rasterizerDescFront;
	ZeroMemory(&rasterizerDescFront, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerDescFront.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID; // solid fill (as opposed to wireframe)
	rasterizerDescFront.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT; // don't draw back-facing triangles 
																// (clockwise pixels : front-facing, counter-clockwise : back-facing)
	hr = this->device->CreateRasterizerState(
		&rasterizerDescFront, this->rasterizerFront.GetAddressOf());

	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create rasterizer state");
		return false;
	}

	/* BLENDING */

	// Create Blend State
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
	
	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory(&rtbd, sizeof(D3D11_RENDER_TARGET_BLEND_DESC));
	
	rtbd.BlendEnable = true;	// enable blending

	// (Final color) = [(Source pixel) * (Source Blend Factor)] + [(Destination pixel) * (Destination Blend Factor)]
	// Multiply the source pixel color by its alpha, and add it to the destination color multiplied by the inverse of the source alpha
	// ex: source:(0, 1, 0, .9), dest:(1,0,0,1) ... final color is (0, (1 * 0.9), 0) + ((1 * (1-.9)), 0, 0) = (0,.9,0) + (.1,0,0) = (0.1, 0.9, 0)
	rtbd.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;	// base the source color on the source alpha
	rtbd.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;	// base the destination alpha on the source alpha
	rtbd.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;	// add the two colors together

	// (Final alpha) = [(Source alpha) * (Source blend factor)] + [(Destination alpha) * (Destination blend factor)]
	// With values set below...	(Final alpha) =	(Source alpha)
	rtbd.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;	// multiply source-blend-alpha by 1 (same thing as it already was)
	rtbd.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;	// Do not take into account (looking at equation above, this value being set to 0 makes the dest. blend alpha meaningless)
	rtbd.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;	// add the two alphas together
	rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;	// take into account red, green, blue, and alpha levels (all)

	

	

	blendDesc.RenderTarget[0] = rtbd; // only have one render target

	hr = device->CreateBlendState(&blendDesc, blendState.GetAddressOf()); // Create the blend state from the description
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create blend state.");
		return false;
	}

	/* FONTS */

	// Init fonts
	spriteBatch = std::make_unique<dx::SpriteBatch>(this->deviceContext.Get());
	spriteFont = std::make_unique<dx::SpriteFont>(this->device.Get(), L"Data/Fonts/proggy_clean_16.spritefont");

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
		// Front
		Vertex(-0.5f,  -0.5f, -0.5f, 0.0f, 1.0f),	//Bottom Left - [0]
		Vertex(-0.5f,   0.5f, -0.5f, 0.0f, 0.0f),	//Top Left - [1]
		Vertex(0.5f,   0.5f, -0.5f, 1.0f, 0.0f),		//Top Right - [2]
		Vertex(0.5f,  -0.5f, -0.5f, 1.0f, 1.0f),		//Bottom Right - [3]

		// Back
		Vertex(-0.5f,  -0.5f, 0.5f, 0.0f, 1.0f),	//Bottom Left - [4]
		Vertex(-0.5f,   0.5f, 0.5f, 0.0f, 0.0f),	//Top Left - [5]
		Vertex(0.5f,   0.5f, 0.5f, 1.0f, 0.0f),		//Top Right - [6]
		Vertex(0.5f,  -0.5f, 0.5f, 1.0f, 1.0f),		//Bottom Right - [7]
	};

	/* VERTEX BUFFER */

	// Create vertex buffer
	HRESULT hr = this->vertexBuffer.initialize(this->device.Get(), v, ARRAYSIZE(v));

	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create vertex buffer");
		return false;
	}

	/* INDICES BUFFER (of vertices) */

	// Create Indices Array
	DWORD indices[]
	{
		// FRONT
		0,1,2,
		0,2,3,
		// LEFT
		4,5,1,
		4,1,0,
		// RIGHT
		3,2,6,
		3,6,7,
		// TOP
		1,5,6,
		1,6,2,
		// BOTTOM
		3,7,4,
		3,4,0,
		// BACK
		7,6,5,
		7,5,4,

	};// using indices fixes reusing multiple instances of the same vertex data when drawing pixels
	
	// Create description for indices buffer
	hr = indicesBuffer.initialize(device.Get(), indices, ARRAYSIZE(indices));

	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create indices buffer.");
		return false;
	}

	/* TEXTURE LOADING */

	// Load face texture
	hr = dx::CreateWICTextureFromFile(
		this->device.Get(), L"Data/Textures/boi.png", nullptr, face.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create wic texture from file");
		return false;
	}

	// Load blue texture
	hr = dx::CreateWICTextureFromFile(
		this->device.Get(), L"Data/Textures/blue.png", nullptr, blue.GetAddressOf()); 
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create wic texture from file");
		return false;
	}

	// Load bird texture
	hr = dx::CreateWICTextureFromFile(
		this->device.Get(), L"Data/Textures/bird.png", nullptr, bird.GetAddressOf()); 
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create wic texture from file");
		return false;
	}

	// Load water texture
	hr = dx::CreateWICTextureFromFile(
		this->device.Get(), L"Data/Textures/water.png", nullptr, water.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create wic texture from file");
		return false;
	}

	/* CONSTANT BUFFER(S) */
	hr = cb_vs_vertexshader.initialize(device.Get(), deviceContext.Get());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to init constant buffer");
		return false;
	}
	hr = cb_ps_pixelshader.initialize(device.Get(), deviceContext.Get());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to init constant buffer");
		return false;
	}

	cam.setPosition(0.0f, 0.0f, -2.0f);
	cam.setProjectionValues(90.0f /*FOV*/, (float)width / (float)height /*Aspect Ratio*/, 0.1f/*near Z*/, 1000.0f/*far Z*/);

	return true;
}
