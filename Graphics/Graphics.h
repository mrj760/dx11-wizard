#pragma once
#include "AdapterReader.h"
#include "Shaders.h"
#include "Vertex.h"

namespace mwrl = Microsoft::WRL;

class Graphics
{
public:
	bool initialize(HWND hwnd, int width, int height); // returns whether initialization of DirectX was successful
	void renderFrame();
private:
	bool initializeShaders();
	bool initializeDirectX(HWND hwnd, int width, int height); // does the actual initialization
	bool initializeScene();

	// device
	mwrl::ComPtr<ID3D11Device> device;
	mwrl::ComPtr<ID3D11DeviceContext> deviceContext; // decides when to set shader resource to drawing textures

	// swapchain
	mwrl::ComPtr<IDXGISwapChain> swapChain; // swaps out frames while rendering is being done
								/* While one frame is displayed to the screen (front buffer, Frame 1)
									another frame is being drawn (back buffer, Frame 2).
									When Frame 2 is ready, Frame 1 is discarded, Frame 2 is sent to front buffer to be displayed, 
									and Frame 3 is sent to rear buffer to be drawn. */
	
	// render target view
	mwrl::ComPtr <ID3D11RenderTargetView> renderTargetView;
	
	// shaders
	VertexShader vertexshader;
	PixelShader pixelshader;

	// vertex buffer(s)
	mwrl::ComPtr <ID3D11Buffer> vertexBuffer;
	mwrl::ComPtr <ID3D11Buffer> vertexBuffer2;

	// depth stencil
	mwrl::ComPtr<ID3D11DepthStencilView> depthStencilView;
	mwrl::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	mwrl::ComPtr<ID3D11DepthStencilState> depthStencilState;

	// rasterizer state
	mwrl::ComPtr<ID3D11RasterizerState> rasterizerState;
};

