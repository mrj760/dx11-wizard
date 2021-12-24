#pragma once
#include <DirectXMath.h>
namespace dx = DirectX;

struct CB_VS_vertexshader // (Constant-Buffer Vertex-Shader)
{
	dx::XMMATRIX mat;
};
// 8 bytes -> 16 bytes