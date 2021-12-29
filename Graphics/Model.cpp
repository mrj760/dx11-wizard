#include "Model.h"

bool Model::initialize(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11ShaderResourceView* texture, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	this->device = device;
	this->context = context;
	this->texture = texture;
	this->cb_vs_vertexshader = &cb_vs_vertexshader;

	try
	{
		// Textured Square
		Vertex v[] =
		{
			// Front
			Vertex(-0.5f,  -0.5f, -0.5f, 0.0f, 1.0f),	//Bottom Left - [0]
			Vertex(-0.5f,   0.5f, -0.5f, 0.0f, 0.0f),	//Top Left - [1]
			Vertex(0.5f,   0.5f, -0.5f, 1.0f, 0.0f),		//Top Right - [2]
			Vertex(0.5f,  -0.5f, -0.5f, 1.0f, 1.0f),		//Bottom Right - [3]

			// Back
			Vertex(-0.5f,  -0.5f, 0.5f, 0.0f, 1.0f),	//Bottom Left - [4]
			Vertex(-0.5f,   0.5f, 0.5f, 0.0f, 0.0f),	//Top Left - [5]
			Vertex(0.5f,   0.5f, 0.5f, 1.0f, 0.0f),		//Top Right - [6]
			Vertex(0.5f,  -0.5f, 0.5f, 1.0f, 1.0f),		//Bottom Right - [7]
		};

		/* VERTEX BUFFER */

		// Create vertex buffer
		HRESULT hr = this->vertexBuffer.initialize(this->device, v, ARRAYSIZE(v));
		COM_ERROR_IF_FAILED(hr, "Failed to create vertex buffer");

		/* INDICES BUFFER (of vertices) */

		// Create Indices Array
		DWORD indices[]
		{
			// FRONT
			0,1,2,
			0,2,3,
			// LEFT
			4,5,1,
			4,1,0,
			// RIGHT
			3,2,6,
			3,6,7,
			// TOP
			1,5,6,
			1,6,2,
			// BOTTOM
			4,0,3,
			4,3,7,
			// BACK
			7,6,5,
			7,5,4,

		};// using indices fixes reusing multiple instances of the same vertex data when drawing pixels

		// Create description for indices buffer
		hr = indexBuffer.initialize(device, indices, ARRAYSIZE(indices));
		COM_ERROR_IF_FAILED(hr, "Failed to create indices buffer");
	}
	catch (COMException& e)
	{
		ErrorLogger::Log(e);
		return false;
	}

	this->updateWorldMatrix();

	return true;
}

void Model::setTexture(ID3D11ShaderResourceView* texture)
{
	this->texture = texture;
}

void Model::draw(const XMMATRIX& viewProjMatrix)
{
	// Update constant buffer with WVP Matrix		(world view projection matrix)
	cb_vs_vertexshader->data.matx = this->worldMatrix * viewProjMatrix;
	cb_vs_vertexshader->data.matx = XMMatrixTranspose(this->cb_vs_vertexshader->data.matx); // change to column-major format
	cb_vs_vertexshader->ApplyChanges();
	context->VSSetConstantBuffers(0, 1, cb_vs_vertexshader->getAddressOf()); // Use the updated constant buffer

	context->PSSetShaderResources(0, 1, &this->texture); // Set texture
	context->IASetIndexBuffer(indexBuffer.get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, vertexBuffer.getAddressOf(), vertexBuffer.stridePtr(), &offset);
	context->DrawIndexed(this->indexBuffer.getBufferSize(), 0, 0);
}

void Model::updateWorldMatrix()
{
	worldMatrix = XMMatrixIdentity();
}
