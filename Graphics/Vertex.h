#pragma once
#include <DirectXMath.h> // 

namespace dx = DirectX;

struct Vertex
{
	Vertex();
	Vertex(float x, float y, float z, float u, float v) : 
		pos(x, y, z), texCoord(u, v) {}
	dx::XMFLOAT3 pos;
	dx::XMFLOAT2 texCoord;
};

