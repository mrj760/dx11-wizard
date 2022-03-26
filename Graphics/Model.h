#pragma once
#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"

using namespace DirectX;
typedef XMMATRIX mx;
typedef XMVECTOR vec;
typedef XMFLOAT3 f3;

class Model
{
public:
	// init needs: device, context, texture, constant buffer
	bool initialize(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11ShaderResourceView* texture, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader);
	// change the assigned texture
	void setTexture(ID3D11ShaderResourceView* texture);
	void draw(const XMMATRIX& viewProjMatrix);

	// Look-At Set
	void setTargetPos(f3 targetPos);

	bool equals(f3 a, f3 b);

	// Get Direction vectors
	const vec& getForward();
	const vec& getBackward();
	const vec& getRight();
	const vec& getLeft();
	const vec& getUp();
	const vec& getDown();

	// Position Gets
	const vec& getPositionVector() const;
	const f3& getpositionFloat3() const;

	// Rotation Gets
	const vec& getRotationVector() const;
	const f3& getRotationFloat() const;

	// Position Sets
	void setPosition(const vec& pos);
	void setPosition(float x, float y, float z);

	// Position Adjustment
	void adjustPosition(const vec& pos);
	void adjustPosition(const f3& pos);
	void adjustPosition(float x, float y, float z);

	// Rotation Sets
	void setRotation(const vec& rot);
	void setRotation(const f3& pos);
	void setRotation(float x, float y, float z);

	// Adjustment Rotations
	void adjustRotation(const vec& rot);
	void adjustRotation(const f3& pos);
	void adjustRotation(float x, float y, float z);

private:

	void updateWorldMatrix();

	ID3D11Device* device;
	ID3D11DeviceContext* context;
	ConstantBuffer<CB_VS_vertexshader>* cb_vs_vertexshader;
	ID3D11ShaderResourceView* texture;

	VertexBuffer<Vertex> vertexBuffer;
	IndexBuffer indexBuffer;

	XMMATRIX worldMatrix = XMMatrixIdentity();

	// Position values
	vec posVec;
	f3 pos;

	// Rotation Values
	vec rotVec;
	f3 rot;

	// view matrix
	mx viewMatrix;

	// projection matrix
	mx projMatrix;

	// constant values. Left handed coordinates so +z forward, +y up
	const vec
		DEFAULT_FORWARD_VECTOR = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f),
		DEFAULT_BACKWARD_VECTOR = XMVectorSet(0.0, 0.0f, -1.0f, 0.0f),
		DEFAULT_LEFT_VECTOR = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f),
		DEFAULT_RIGHT_VECTOR = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f),
		DEFAULT_UP_VECTOR = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f),
		DEFAULT_DOWN_VECTOR = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);

	vec
		forward, left, right, backward, up, down;
};

