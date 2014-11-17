#pragma once
#include "mathlib.h"

class Camera
{
public:
	Camera();
	Camera(Vector3 pos, Vector3 lookat, Vector3 up);
	~Camera();
	void roll(float angleDeg);
	void pitch(float angleDeg);
	void yaw(float angleDeg);
	void move(float distance);
	void strafe(float distance);
	void climb(float distance);
	Matrix4 getViewMatrix();
	void setPosition(Vector3 _position) { position = _position; }
	Vector3 getPosition() { return position; };

protected:
	void updateOrientation(Vector3 axis, float angleDeg);
	Vector3 upVector;
	Vector3 forwardVector;
	Vector3 position;
};