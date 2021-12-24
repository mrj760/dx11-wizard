#pragma once
#include <DirectXMath.h>
using namespace DirectX;
typedef XMMATRIX mx;
typedef XMVECTOR vec;
typedef XMFLOAT3 f3;

class Camera
{
public:
	Camera(); // init camera to center of world with no rotation
	
	// set projection matrix values
	void setProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ); 

	// View Matrix Get
	const mx& getViewMatrix() const;
	// Projection Matrix Get
	const mx& getProjectionMatrix() const;

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
	void adjustPosition(float x, float y, float z);

	// Rotation Sets
	void setRotation(const vec& rot);
	void setRotation(float x, float y, float z);

	// Adjustment Rotations
	void adjustRotation(const vec& rot);
	void adjustRotation(float x, float y, float z);


private:
	// Update the view matrix whenever position/rotation modified
	void updateViewMatrix();

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
	const vec DEFAULT_FORWARD_VECTOR = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const vec DEFAULT_UP_VECTOR = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
};

