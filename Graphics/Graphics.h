/*
*	GRAPHICS MANAGER
*		Initializes DirectX, Shaders, Scenes
*		Updates graphics buffers
*		Renders frames
*		Uses handles to windows
*/

#pragma once
#include "AdapterReader.h"
#include "Shaders.h"
#include "Vertex.h"
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <WICTextureLoader.h>
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "ConstantBufferTypes.h"
#include "Camera.h"
#include "../Base/Timer.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"

namespace mwrl = Microsoft::WRL;
namespace dx = DirectX;

class Graphics
{

public:
	bool initialize(HWND hwnd, int width, int height); // returns whether initialization of DirectX was successful
	void renderFrame();
	// Camera
	Camera cam;

private:
	bool initializeShaders();
	bool initializeDirectX(HWND hwnd); // does the actual initialization
	bool initializeScene();

	// Window information
	int width=0, height=0; // width and height

	Timer timer;

	// Device
	mwrl::ComPtr<ID3D11Device> device;
	mwrl::ComPtr<ID3D11DeviceContext> deviceContext; // decides when to set shader resource to drawing textures

	// Swapchain
	mwrl::ComPtr<IDXGISwapChain> swapChain; // swaps out frames while rendering is being done
								/* While one frame is displayed to the screen (front buffer, Frame 1)
									another frame is being drawn (back buffer, Frame 2).
									When Frame 2 is ready, Frame 1 is discarded, Frame 2 is sent to front buffer to be displayed, 
									and Frame 3 is sent to rear buffer to be drawn. */
	
	// Render target view
	mwrl::ComPtr <ID3D11RenderTargetView> renderTargetView;
	
	// Shaders
	VertexShader vertexshader;
	PixelShader pixelshader;
	ConstantBuffer<CB_VS_vertexshader> cb_vs_vertexshader;
	ConstantBuffer<CB_PS_pixelshader> cb_ps_pixelshader;

	// Vertex buffer(s)
	VertexBuffer<Vertex> vertexBuffer;

	// Indeces buffer for vertex data
	//mwrl::ComPtr<ID3D11Buffer> indecesBuffer;
	IndexBuffer indicesBuffer;

	// Depth stencil
	mwrl::ComPtr<ID3D11DepthStencilView> depthStencilView;
	mwrl::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	mwrl::ComPtr<ID3D11DepthStencilState> depthStencilState;

	// Rasterizer state
	mwrl::ComPtr<ID3D11RasterizerState> rasterizerState;

	// Blend state
	mwrl::ComPtr<ID3D11BlendState> blendState;

	// Font stuff
	std::unique_ptr<dx::SpriteBatch> spriteBatch;
	std::unique_ptr<dx::SpriteFont> spriteFont;

	// Sampler state
	mwrl::ComPtr<ID3D11SamplerState> samplerState;

	// Shader resource (texture)
	mwrl::ComPtr<ID3D11ShaderResourceView> face;
	mwrl::ComPtr<ID3D11ShaderResourceView> bird;
	mwrl::ComPtr<ID3D11ShaderResourceView> water;
	mwrl::ComPtr<ID3D11ShaderResourceView> blue;
};

