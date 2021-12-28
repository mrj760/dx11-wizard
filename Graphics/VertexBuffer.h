/**
*	VERTEX BUFFER
*		Buffer which contains the vertices used to draw geometry
*/

#pragma once
#ifndef VertexBuffer_h__
#define VertexVuffer_h__
#include <d3d11.h>
#include <wrl/client.h>

namespace mwrl = Microsoft::WRL;

template<class T>
class VertexBuffer
{

private:

	VertexBuffer(const VertexBuffer<T>& rhs);

	mwrl::ComPtr<ID3D11Buffer> buffer;
	std::unique_ptr<UINT> stride;
	UINT bufferSize = 0;

public:

	VertexBuffer() {}

	ID3D11Buffer* get() const
	{
		return buffer.Get();
	}

	ID3D11Buffer* const* getAddressOf() const
	{
		return buffer.GetAddressOf();
	}

	UINT getBufferSize() const
	{
		return bufferSize;
	}

	const UINT getStride() const
	{
		return *this->stride.get();
	}

	const UINT* stridePtr() const
	{
		return stride.get();
	}

	HRESULT initialize(ID3D11Device* device, T* data, UINT numVertices)
	{
		if (buffer.Get() != nullptr) // reset the buffer so it can be reused and we don't have to create a new one (mem leak)
			buffer.Reset();
		else // set stride length if this is the first time making the vertex buffer
			stride = std::make_unique<UINT>(sizeof(T));

		bufferSize = numVertices;

		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(T) * bufferSize;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferData;
		ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
		vertexBufferData.pSysMem = data;

		HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, this->buffer.GetAddressOf());
		return hr;
	}
};

#endif