#pragma once
#include <DirectXMath.h> // 

namespace dx = DirectX;

struct Vertex
{
	Vertex();
	Vertex(float x, float y, float r, float g, float b) : 
		pos(x, y), color(r,g,b) {}
	dx::XMFLOAT2 pos;
	dx::XMFLOAT3 color;
};

