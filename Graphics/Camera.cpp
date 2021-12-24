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
// Set where to look
void Camera::setTargetPos(f3 targetPos)
{
	// cannot look at self (undefined math)
	if (equals(targetPos, pos))
		return;

	// Calculate difference vect. from current position to look-at target
	targetPos.x = pos.x - targetPos.x;
	targetPos.y = pos.y - targetPos.y;
	targetPos.z = pos.z - targetPos.z;

	// find up/down pitch for camera look angle
	float pitch = 0.0f;
	if (targetPos.y != 0.0f)
	{
		// See Notes folder for "3d_camera_pitch_atan_note.png"
		const float dist = sqrt(targetPos.x * targetPos.x + targetPos.z * targetPos.z);
		pitch = atan(targetPos.y / dist); // atan(sqrt(x^2 + z^2)) radians looking up/down
	}

	// find left/right yaw for camera look angle
	float yaw = 0.0f;
	if (targetPos.x != 0.0f);
	{
		// See Notes folder for "3d_camera_yaw_atan_note.png"
		yaw = atan(targetPos.x / targetPos.z);
	}
	if (targetPos.z > 0)
	{
		// if in the positive z space, camera must turn more than 180 degrees to face origin. See Note mentioned above.
		yaw += XM_PI;
	}

	// apply the rotation
	setRotation(pitch, yaw, 0.0f);
}

const vec& Camera::getForward()
{
	return forward;
}

const vec& Camera::getLeft()
{
	return left;
}

const vec& Camera::getUp()
{
	return up;
}

const vec& Camera::getDown()
{
	return down;
}

const vec& Camera::getBackward()
{
	return backward;
}

const vec& Camera::getRight()
{
	return right;
}

bool Camera::equals(f3 a, f3 b)
{
	return a.x == b.x && a.y == b.y && a.z == b.z;
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
	vec upDir = XMVector3TransformCoord(DEFAULT_UP_VECTOR, camRotMatrix);
	
	// Rebuild view matrix with position, target and up
	viewMatrix = XMMatrixLookAtLH(posVec, camTarget, upDir);

	// Update fwd/backward/left/right vectors
	mx rotMatrix = XMMatrixRotationRollPitchYaw(rot.x, rot.y, 0.0f); // always want roll to be the same.

	forward = XMVector3TransformCoord(DEFAULT_FORWARD_VECTOR, rotMatrix);
	backward = XMVector3TransformCoord(DEFAULT_BACKWARD_VECTOR, rotMatrix);
	right = XMVector3TransformCoord(DEFAULT_RIGHT_VECTOR, rotMatrix);
	left = XMVector3TransformCoord(DEFAULT_LEFT_VECTOR, rotMatrix);
	up = XMVector3TransformCoord(DEFAULT_UP_VECTOR, rotMatrix);
	down = XMVector3TransformCoord(DEFAULT_DOWN_VECTOR, rotMatrix);
}
