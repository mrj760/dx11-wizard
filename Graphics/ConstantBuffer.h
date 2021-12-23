/**
*	CONSTANT BUFFER: 
		Preserves values of stored shader constants until it becomes necessary to change them.

		Holds a buffer and a device context.

*/

#pragma once
#ifndef ConstantBuffer_h__
#define ConstantBuffer_h__

#include <d3d11.h>
#include "ConstantBufferTypes.h"
#include <wrl/client.h>
#include "../Base/ErrorLogger.h"

namespace mwrl = Microsoft::WRL;

template<class T>
class ConstantBuffer
{

private:

	ConstantBuffer(const ConstantBuffer<T>& rhs);

	mwrl::ComPtr<ID3D11Buffer> buffer;
	ID3D11DeviceContext* deviceContext; // need device context to map and unmap

public:

	ConstantBuffer() {}

	T data; // data type which we're working with

	ID3D11Buffer* get() const
	{
		return buffer.Get();
	}

	ID3D11Buffer* const* getAddressOf() const
	{
		return buffer.GetAddressOf();
	}

	HRESULT initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	{
		this->deviceContext = deviceContext; // save the device context

		// Describe constant buffer
		D3D11_BUFFER_DESC constantBufferDesc;
		ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC)); // yes/no?
		constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constantBufferDesc.MiscFlags = 0;
		constantBufferDesc.ByteWidth = 
			static_cast<UINT>(sizeof(T) + 
				(16 - (sizeof(T) % 16))); // align to 16 bytes size
		constantBufferDesc.StructureByteStride = 0;

		// Create constant buffer
		HRESULT hr = device->CreateBuffer(
			&constantBufferDesc, 
			0,  
			this->buffer.GetAddressOf()); 
		return hr;
	}

	// Update for mapping and unmapping
	bool ApplyChanges()
	{
		D3D11_MAPPED_SUBRESOURCE mappedRes;

		// Map: Gets a pointer to the data contained in a subresource, and denies the GPU access to that subresource.
		HRESULT hr = this->deviceContext->Map(
			buffer.Get(), // A pointer to a ID3D11Resource interface.
			0, // Index number of the subresource.
			D3D11_MAP_WRITE_DISCARD, // A D3D11_MAP-typed value that specifies the CPU's read and write permissions for a resource.
			0, // Flag that specifies what the CPU does when the GPU is busy. This flag is optional.
			&mappedRes); // A pointer to the D3D11_MAPPED_SUBRESOURCE structure for the mapped subresource.
		if (FAILED(hr))
		{
			ErrorLogger::Log(hr, "Failed to map to constant buffer");
			return false;
		}
		// Success
		CopyMemory(mappedRes.pData, &data, sizeof(T));
		this->deviceContext->Unmap(buffer.Get(), 0);
		
		return true;
	}
};

#endif // ConstantBuffer_h__