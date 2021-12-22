#pragma once
#include <DirectXMath.h> // 

namespace dx = DirectX;

struct Vertex
{
	Vertex();
	Vertex(float x, float y, float z, float r, float g, float b) : 
		pos(x, y, z), color(r,g,b) {}
	dx::XMFLOAT3 pos;
	dx::XMFLOAT3 color;
};

