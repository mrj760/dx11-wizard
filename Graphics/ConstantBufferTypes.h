#pragma once
#include <DirectXMath.h>
namespace dx = DirectX;

struct CB_VS_vertexshader // (Constant-Buffer Vertex-Shader)
{
	dx::XMMATRIX matx;
};
// 8 bytes -> 16 bytes

struct CB_PS_pixelshader // (Constant-Buffer Pixel-Shader)
{
	float alpha = 1.0f;
};