#include "Camera.h"
#include <stdio.h>

Camera::Camera()
{

}

Camera::Camera(Vector3 pos, Vector3 lookat, Vector3 up)
{
	position = pos;
	forwardVector = lookat - pos;
	forwardVector.normalize();
	upVector = up;
	upVector.normalize();
}

Camera::~Camera()
{
}

void Camera::roll(float angleDeg){
	updateOrientation(forwardVector, angleDeg);
}

void Camera::pitch(float angleDeg){
	updateOrientation(Vector3::cross(forwardVector, upVector), angleDeg);
}

void Camera::yaw(float angleDeg){
	//updateOrientation(upVector, angleDeg);
	updateOrientation(Vector3(0,1,0), angleDeg);
}

void Camera::move(float distance){
	position += forwardVector * distance;
}

void Camera::strafe(float distance){
	position += Vector3::cross(forwardVector, upVector) * distance;
}

void Camera::climb(float distance){
	position += upVector * distance;
}

void Camera::updateOrientation(Vector3 axis, float angleDeg){
	Matrix4 rotMatrix = Matrix4::IDENTITY;
	rotMatrix.rotate(axis, angleDeg);

	Vector3 newUpVector, newForwardVector;

	newUpVector = upVector * rotMatrix;
	newForwardVector = forwardVector * rotMatrix;

	newUpVector.normalize();
	newForwardVector.normalize();
	upVector = newUpVector;
	forwardVector = newForwardVector;
}

Matrix4 Camera::getViewMatrix(){
	Matrix4 viewMatrix;
	Matrix4 rotation = Matrix4::IDENTITY, translation = Matrix4::IDENTITY;
	Vector3 right, up;

	forwardVector.normalize();

	right = Vector3::cross(forwardVector, upVector);
	right.normalize();

	up = Vector3::cross(right, forwardVector);
	up.normalize();

	rotation = Matrix4(right.x, right.y, right.z, 0,
		up.x, up.y, up.z, 0,
		-forwardVector.x, -forwardVector.y, -forwardVector.z, 0,
		0, 0, 0, 1);

	translation[0][3] = -position.x;
	translation[1][3] = -position.y;
	translation[2][3] = -position.z;

	viewMatrix = rotation * translation;
	return viewMatrix;
}