#pragma once
#ifndef IndicesBuffer_h__
#define IndicesBuffer_h__
#include <d3d11.h>
#include <wrl/client.h>
#include <vector>

namespace mwrl = Microsoft::WRL;

class IndexBuffer
{
private:
	IndexBuffer(const IndexBuffer& rhs);

private:
	mwrl::ComPtr<ID3D11Buffer> buffer;
	UINT bufferSize = 0;

public:
	IndexBuffer() {}

	ID3D11Buffer* get() const
	{
		return buffer.Get();
	}

	ID3D11Buffer* const* getAddressOf()
	{
		return buffer.GetAddressOf();
	}

	UINT getBufferSize() const
	{
		return bufferSize;
	}

	HRESULT initialize(ID3D11Device* device, DWORD* data, UINT numIndeces)
	{
		if (buffer.Get() != nullptr) // reset the buffer if not null, avoid memory leak caused by init'ing new buffer on top of the current one
			buffer.Reset();

		bufferSize = numIndeces;

		D3D11_BUFFER_DESC indexBufferDesc;
		ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(DWORD) * numIndeces;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA indexBufferData;
		indexBufferData.pSysMem = data;
		
		return device->CreateBuffer(&indexBufferDesc, &indexBufferData, buffer.GetAddressOf());
	}
};

#endif // !IndecesBuffer_h__
