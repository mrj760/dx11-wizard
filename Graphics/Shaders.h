#pragma once
#include "../Base/ErrorLogger.h"
#pragma comment(lib, "D3DCompiler.lib")
#include <d3d11.h>
#include <wrl/client.h>
#include <d3dcompiler.h>

 namespace mwrl = Microsoft::WRL;

class VertexShader
{
public:
	bool initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderpath, D3D11_INPUT_ELEMENT_DESC* desc, UINT numElements);
	ID3D11VertexShader* getShader();
	ID3D10Blob* getBuffer();
	ID3D11InputLayout* getInputLayout();
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> shader;
	Microsoft::WRL::ComPtr<ID3D10Blob> shader_buffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
};

class PixelShader
{
public:
	bool initialize(mwrl::ComPtr<ID3D11Device>& device, std::wstring shaderpath);
	ID3D11PixelShader* getShader();
	ID3D10Blob* getBuffer();
private:
	mwrl::ComPtr<ID3D11PixelShader> shader;
	mwrl::ComPtr<ID3D10Blob> shader_buffer;
};

