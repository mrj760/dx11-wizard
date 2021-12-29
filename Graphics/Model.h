#pragma once
#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"

using namespace DirectX;

class Model
{
public:
	// init needs: device, context, texture, constant buffer
	bool initialize(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11ShaderResourceView* texture, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader);
	// change the assigned texture
	void setTexture(ID3D11ShaderResourceView* texture);
	void draw(const XMMATRIX& viewProjMatrix);

private:

	void updateWorldMatrix();

	ID3D11Device* device;
	ID3D11DeviceContext* context;
	ConstantBuffer<CB_VS_vertexshader>* cb_vs_vertexshader;
	ID3D11ShaderResourceView* texture;

	VertexBuffer<Vertex> vertexBuffer;
	IndexBuffer indexBuffer;

	XMMATRIX worldMatrix = XMMatrixIdentity();
};

