#include "Camera.h"

Camera::Camera()
{
	pos = f3(0.0f, 0.0f, 0.0f);
	posVec = XMLoadFloat3(&pos);

	rot = f3(0.0f, 0.0f, 0.0f);
	rotVec = XMLoadFloat3(&rot);

	updateViewMatrix();
}

void Camera::setProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	float fovRad = (fovDegrees / 360.0f) * XM_2PI;
	
	projMatrix = XMMatrixPerspectiveFovLH(fovRad, aspectRatio, nearZ, farZ);
}

const mx& Camera::getViewMatrix() const
{
	return viewMatrix;
}

const mx& Camera::getProjectionMatrix() const
{
	return projMatrix;
}

const vec& Camera::getPositionVector() const
{
	return posVec;
}

const f3& Camera::getpositionFloat3() const
{
	return pos;
}

const vec& Camera::getRotationVector() const
{
	return rotVec;
}

const f3& Camera::getRotationFloat() const
{
	return rot;
}

void Camera::setPosition(const vec& pos)
{
	XMStoreFloat3(&this->pos, pos);
	posVec = pos;

	updateViewMatrix();
}

void Camera::setPosition(float x, float y, float z)
{
	pos = f3(x, y, z);
	posVec = XMLoadFloat3(&pos);

	updateViewMatrix();
}

void Camera::adjustPosition(const vec& pos)
{
	posVec += pos;
	XMStoreFloat3(&this->pos, posVec);

	updateViewMatrix();
}

void Camera::adjustPosition(float x, float y, float z)
{
	pos = f3(pos.x + x, pos.y + y, pos.z + z);
	posVec = XMLoadFloat3(&pos);

	updateViewMatrix();
}

void Camera::setRotation(const vec& rot)
{
	rotVec = rot;
	XMStoreFloat3(&this->rot, rot);
}

void Camera::setRotation(float x, float y, float z)
{
	rot = f3(x, y, z);
	rotVec = XMLoadFloat3(&rot);

	updateViewMatrix();
}

void Camera::adjustRotation(const vec& rot)
{
	rotVec += rot;
	XMStoreFloat3(&this->rot, rot);

	updateViewMatrix();
}

void Camera::adjustRotation(float x, float y, float z)
{
	rot = f3(rot.x + x, rot.y + y, rot.z + z);
	rotVec = XMLoadFloat3(&rot);

	updateViewMatrix();
}

// Updates view matrix and movement vectors
void Camera::updateViewMatrix()
{
	// Calc. camera rotation matrix
	mx camRotMatrix = XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);

	// Calc. unit vect of cam target based on cam. fwd value transformed by cam. rot.
	vec camTarget = XMVector3TransformCoord(DEFAULT_FORWARD_VECTOR, camRotMatrix);
	// Offset cam target by cam.'s current pos.
	camTarget += posVec;

	// Calc. up direction based on cam.'s current rot.
	vec up = XMVector3TransformCoord(DEFAULT_UP_VECTOR, camRotMatrix);
	
	// Rebuild view matrix with position, target and up
	viewMatrix = XMMatrixLookAtLH(posVec, camTarget, up);
}
