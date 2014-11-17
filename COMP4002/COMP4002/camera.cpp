////////////////////////////////////////////////////////////////////////////////
// Filename: cameraclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "camera.h"

Camera::Camera() {

	m_position = Vector3(0.0f, 0.0f, 0.0f);
	m_lookatPosition = Vector3(0.0f, 0.0f, 0.0f);

	follow = 0;
	//CalculatePosition();
	m_quatOrientation.identity();
	//m_viewMatrix.identity();
	upToDate = false;
	firstPerson = true;
}

Camera::Camera(Vector3 pos) {

	follow = 0;
	//CalculatePosition();
	m_quatOrientation.identity();
	//m_viewMatrix.identity();
	m_position = pos;
	m_lookatPosition = pos + Vector3(0.0f, 0.0f, -1.0f);
	upToDate = false;
	firstPerson = true;
}


Camera::Camera(const Camera& other) { }


Camera::~Camera() { }


void Camera::setPosition(float x, float y, float z) {
	m_position = Vector3(x, y, z);
	upToDate = false;
	return;
}


void Camera::setPosition(Vector3 v) {
	m_position = v;
	upToDate = false;
	return;
}


void Camera::setOrientation(Quaternion quatOrient) {
	m_quatOrientation = quatOrient;
	upToDate = false;
	return;
}

void Camera::update() {
	Matrix4 matTranslation;

	matTranslation = Matrix4(1, 0, 0, -m_position.x,
		0, 1, 0, -m_position.y,
		0, 0, 1, -m_position.z,
		0, 0, 0, 1);

	// 1.2) Now calculate rotation, by taking the conjucate of the quaternion
	Matrix4 matRotation;
	matRotation = m_quatOrientation.inverse().toMatrix4();

	// 2) Apply rotation & translation matrix at view matrix
	m_viewMatrix = matRotation * matTranslation;

	upToDate = true;
}

Matrix4 Camera::getViewMatrix()
{
	if (!upToDate) update();
	return m_viewMatrix;
}

const Vector3 Camera::getAxisX() const
{
	Vector3 vAxis;

	vAxis.x = m_viewMatrix[0][0];
	vAxis.y = m_viewMatrix[0][1];
	vAxis.z = m_viewMatrix[0][2];

	return vAxis;
}

const Vector3 Camera::getAxisY() const
{
	Vector3 vAxis;

	vAxis.x = m_viewMatrix[1][0];
	vAxis.y = m_viewMatrix[1][1];
	vAxis.z = m_viewMatrix[1][2];

	return vAxis;
}

const Vector3 Camera::getAxisZ() const
{
	Vector3 vAxis;

	vAxis.x = m_viewMatrix[2][0];
	vAxis.y = m_viewMatrix[2][1];
	vAxis.z = m_viewMatrix[2][2];

	return vAxis;
}

void Camera::applyTranslation(float fDistance, eDir ceDir) {
	Vector3 vDir;

	switch (ceDir) {
		case MOVE: {
					   vDir = getAxisZ();
					   break;
		}
		case STRAFE: {
						 vDir = getAxisX();
						 break;
		}
		case UPWARDS: {
						  vDir = getAxisY();
						  break;
		}
	}

	m_position += vDir * fDistance;
	m_lookatPosition += vDir * fDistance;

	upToDate = false;

	return;
}

bool Camera::rotate(Quaternion *pOrientation, Vector3 vec, float fAngle){
	bool bSuccess = false;

	if (pOrientation) {
		Quaternion Rotation;

		Rotation.fromAxisAngle(*transformVector(
			pOrientation,
			&vec
			), fAngle);

		*pOrientation = Rotation * *pOrientation;

		bSuccess = true;
	}

	return bSuccess;
}

bool Camera::rotateXAxis(Quaternion *pOrientation, float fAngle) {
	bool bSuccess = false;
	bSuccess = rotate(pOrientation, Vector3(1.0f, 0.0f, 0.0f), fAngle);
	return bSuccess;
}

bool Camera::rotateYAxis(Quaternion *pOrientation, float fAngle) {
	bool bSuccess = false;
	if (firstPerson) {
		Quaternion Rotation;
		Rotation.fromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), fAngle);
		*pOrientation = Rotation * *pOrientation;
		bSuccess = true;
	}
	else
	{
		bool bSuccess = false;
		bSuccess = rotate(pOrientation, Vector3(1.0f, 0.0f, 0.0f), fAngle);
	}
	return bSuccess;
}

bool Camera::rotateZAxis(Quaternion *pOrientation, float fAngle) {
	bool bSuccess = false;
	bSuccess = rotate(pOrientation, Vector3(0.0f, 0.0f, 1.0f), fAngle);
	return bSuccess;
}

void Camera::applyRotate(float fAngle, eOrient oeOrient) {
	switch (oeOrient) {
	case PITCH: {
					rotateXAxis(&m_quatOrientation, fAngle);
					break;
	}
	case YAW: {
				  rotateYAxis(&m_quatOrientation, fAngle);
				  break;
	}
	case ROLL: {
				   rotateZAxis(&m_quatOrientation, fAngle);
				   break;
	}
	}

	m_quatOrientation.normalize();

	upToDate = false;

	return;
}

Vector3* Camera::transformVector(Quaternion *pOrientation, Vector3 *pAxis) {
	Vector3 vNewAxis;
	Matrix4 matRotation;
	
	// Build a matrix from the quaternion.
	matRotation = pOrientation->toMatrix4();

	// Transform the queried axis vector by the matrix.
	vNewAxis.x = (pAxis->x * matRotation[0][0]) + (pAxis->y * matRotation[0][1]) + (pAxis->z * matRotation[0][2]) + matRotation[0][3];
	vNewAxis.y = (pAxis->x * matRotation[1][0]) + (pAxis->y * matRotation[1][1]) + (pAxis->z * matRotation[1][2]) + matRotation[1][3];
	vNewAxis.z = (pAxis->x * matRotation[2][0]) + (pAxis->y * matRotation[2][1]) + (pAxis->z * matRotation[2][2]) + matRotation[2][3];

	pAxis->x = vNewAxis.x;
	pAxis->y = vNewAxis.y;
	pAxis->z = vNewAxis.z;
	return pAxis;
}