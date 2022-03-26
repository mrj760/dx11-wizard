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


	this->setPosition(0.f, 0.f, 0.f);
	this->setRotation(0.f, 0.f, 0.f);
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

const vec& Model::getPositionVector() const
{
	return posVec;
}

const f3& Model::getpositionFloat3() const
{
	return pos;
}

const vec& Model::getRotationVector() const
{
	return rotVec;
}

const f3& Model::getRotationFloat() const
{
	return rot;
}

void Model::setPosition(const vec& pos)
{
	XMStoreFloat3(&this->pos, pos);
	posVec = pos;

	updateWorldMatrix();
}

void Model::setPosition(float x, float y, float z)
{
	pos = f3(x, y, z);
	posVec = XMLoadFloat3(&pos);

	updateWorldMatrix();
}

void Model::adjustPosition(const vec& pos)
{
	posVec += pos;
	XMStoreFloat3(&this->pos, posVec);

	updateWorldMatrix();
}

void Model::adjustPosition(const f3& pos)
{
	this->pos.x += pos.x;
	this->pos.x += pos.x;
	this->pos.z += pos.z;

	XMStoreFloat3(&this->pos, posVec);
	updateWorldMatrix();
}

void Model::adjustPosition(float x, float y, float z)
{
	pos = f3(pos.x + x, pos.y + y, pos.z + z);

	posVec = XMLoadFloat3(&pos);
	updateWorldMatrix();
}

void Model::setRotation(const vec& rot)
{
	this->rotVec = rot;
	XMStoreFloat3(&this->rot, this->rotVec);
}

void Model::setRotation(const f3& rot)
{
	this->rot = rot;

	XMStoreFloat3(&this->rot, this->rotVec);
	updateWorldMatrix();
}

void Model::setRotation(float x, float y, float z)
{
	this->rot = f3(x, y, z);
	this->rotVec = XMLoadFloat3(&this->rot);

	updateWorldMatrix();
}

void Model::adjustRotation(const vec& rot)
{
	this->rotVec += rot;

	XMStoreFloat3(&this->rot, this->rotVec);
	updateWorldMatrix();
}

void Model::adjustRotation(const f3& rot)
{
	this->rot.x += rot.x;
	this->rot.y += rot.y;
	this->rot.z += rot.z;

	this->rotVec = XMLoadFloat3(&this->rot);
	updateWorldMatrix();
}

void Model::adjustRotation(float x, float y, float z)
{
	rot = f3(rot.x + x, rot.y + y, rot.z + z);

	this->rotVec = XMLoadFloat3(&this->rot);
	updateWorldMatrix();
}
// Set where to look
void Model::setTargetPos(f3 targetPos)
{
	// cannot look at self (undefined math)
	if (equals(targetPos, pos))
		return;

	// Calculate difference vect. from current position to look-at target
	targetPos.x = pos.x - targetPos.x;
	targetPos.y = pos.y - targetPos.y;
	targetPos.z = pos.z - targetPos.z;

	// find up/down pitch for Model look angle
	float pitch = 0.0f;
	if (targetPos.y != 0.0f)
	{
		// See Notes folder for "3d_Model_pitch_atan_note.png"
		const float dist = sqrt(targetPos.x * targetPos.x + targetPos.z * targetPos.z);
		pitch = atan(targetPos.y / dist); // atan(sqrt(x^2 + z^2)) radians looking up/down
	}

	// find left/right yaw for Model look angle
	float yaw = 0.0f;
	if (targetPos.x != 0.0f);
	{
		// See Notes folder for "3d_Model_yaw_atan_note.png"
		yaw = atan(targetPos.x / targetPos.z);
	}
	if (targetPos.z > 0)
	{
		// if in the positive z space, Model must turn more than 180 degrees to face origin. See Note mentioned above.
		yaw += XM_PI;
	}

	// apply the rotation
	setRotation(pitch, yaw, 0.0f);
}

const vec& Model::getForward()
{
	return forward;
}

const vec& Model::getLeft()
{
	return left;
}

const vec& Model::getUp()
{
	return up;
}

const vec& Model::getDown()
{
	return down;
}

const vec& Model::getBackward()
{
	return backward;
}

const vec& Model::getRight()
{
	return right;
}

bool Model::equals(f3 a, f3 b)
{
	return a.x == b.x && a.y == b.y && a.z == b.z;
}

// Updates view matrix and movement vectors
void Model::updateWorldMatrix()
{
	// set world matrix to product of rotation and position
	this->worldMatrix = 
		XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z) 
		* XMMatrixTranslation(this->pos.x, this->pos.y, this->pos.z);

	// base the model's fwd/back/left/right references only off the model's y rotation
	mx vecRotMx = XMMatrixRotationRollPitchYaw(0.f, this->rot.y, 0.f);
	this->forward	= XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, vecRotMx);
	this->backward	= XMVector3TransformCoord(this->DEFAULT_BACKWARD_VECTOR, vecRotMx);
	this->left		= XMVector3TransformCoord(this->DEFAULT_LEFT_VECTOR, vecRotMx);
	this->right		= XMVector3TransformCoord(this->DEFAULT_RIGHT_VECTOR, vecRotMx);
}

