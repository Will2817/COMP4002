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


void Camera::SetPosition(float x, float y, float z) {
	//m_position = Vector3(x, y, z);
	upToDate = false;
	return;
}


void Camera::SetPosition(Vector3 v) {
	//m_position = v;
	upToDate = false;
	return;
}


void Camera::SetOrientation(Quaternion quatOrient) {
	m_quatOrientation = quatOrient;
	upToDate = false;
	return;
}


void Camera::Update() {
	Matrix4 matTranslation;

	//m_lookatPosition = m_position - GetAxisZ();
	//up = GetAxisY();

	matTranslation = Matrix4(1, 0, 0, -m_position.x,
		0, 1, 0, -m_position.y,
		0, 0, 1, -m_position.z,
		0, 0, 0, 1);

	// 1.2) Now calculate rotation, by taking the conjucate of the quaternion
	Matrix4 matRotation;
	matRotation = m_quatOrientation.inverse().toMatrix4();

	// 2) Apply rotation & translation matrix at view matrix
	m_viewMatrix = matTranslation * matRotation;

	upToDate = true;
}

void Camera::display(){
	Update();

	m_lookatPosition = m_position - GetAxisZ();
	up = GetAxisY();
	up.normalize();
	up = Vector3(0.0f, 1.0f, 0.0f);

	gluLookAt(m_position.x,m_position.y,m_position.z,
		      m_lookatPosition.x,m_lookatPosition.y,m_lookatPosition.z,
			  up.x,up.y,up.z);
}

/*
void Camera::GetViewMatrix(Matrix4& viewMatrix)
{
	if (!upToDate) Update();
	viewMatrix = m_viewMatrix;
	return;
}*/

const Vector3 Camera::GetAxisX() const
{
	Vector3 vAxis;

	vAxis.x = m_viewMatrix[0][0];
	vAxis.y = m_viewMatrix[0][1];
	vAxis.z = m_viewMatrix[0][2];

	return vAxis;
}

const Vector3 Camera::GetAxisY() const
{
	Vector3 vAxis;

	vAxis.x = m_viewMatrix[1][0];
	vAxis.y = m_viewMatrix[1][1];
	vAxis.z = m_viewMatrix[1][2];

	return vAxis;
}

const Vector3 Camera::GetAxisZ() const
{
	Vector3 vAxis;

	vAxis.x = m_viewMatrix[2][0];
	vAxis.y = m_viewMatrix[2][1];
	vAxis.z = m_viewMatrix[2][2];

	return vAxis;
}

void Camera::ApplyTranslation(float fDistance, eDir ceDir) {
	Vector3 vDir;

	switch (ceDir) {
		case MOVE: {
					   vDir = GetAxisZ();
					   break;
		}
		case STRAFE: {
						 vDir = GetAxisX();
						 break;
		}
		case UPWARDS: {
						  vDir = GetAxisY();
						  break;
		}
	}
	std::cout << "VDIR: " << vDir.x << "," << vDir.y << "," << vDir.z << "\n";

	m_position += vDir * fDistance;
	m_lookatPosition += vDir * fDistance;

	upToDate = false;

	return;
}

bool Camera::RotateXAxis(Quaternion *pOrientation, float fAngle) {
	bool bSuccess = false;

	if (pOrientation) {
		Quaternion Rotation;

		Rotation.fromAxisAngle(*TransformVector(
			pOrientation,
			&Vector3(1.0f, 0.0f, 0.0f)
			), fAngle);

		*pOrientation = Rotation * *pOrientation;

		bSuccess = true;
	}

	return bSuccess;
}

bool Camera::RotateYAxis(Quaternion *pOrientation, float fAngle) {
	bool bSuccess = false;

	if (pOrientation) {
		Quaternion Rotation;

		if (firstPerson) {
			Rotation.fromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), fAngle);
		}
		else
		{
			Rotation.fromAxisAngle(*TransformVector(
				pOrientation,
				&Vector3(0.0f, 1.0f, 0.0f)
				), fAngle);
		}

		*pOrientation = Rotation * *pOrientation;

		bSuccess = true;
	}

	return bSuccess;
}

bool Camera::RotateZAxis(Quaternion *pOrientation, float fAngle) {
	bool bSuccess = false;

	if (pOrientation) {
		Quaternion Rotation;

		Rotation.fromAxisAngle(*TransformVector(
			pOrientation,
			&Vector3(0.0f, 0.0f, 1.0f)
			), fAngle);

		*pOrientation = Rotation * *pOrientation;

		bSuccess = true;
	}

	return bSuccess;
}

void Camera::ApplyRotate(float fAngle, eOrient oeOrient) {
	switch (oeOrient) {
	case PITCH: {
					RotateXAxis(&m_quatOrientation, fAngle);
					break;
	}
	case YAW: {
				  RotateYAxis(&m_quatOrientation, fAngle);
				  break;
	}
	case ROLL: {
				   RotateZAxis(&m_quatOrientation, fAngle);
				   break;
	}
	}

	m_quatOrientation.normalize();

	upToDate = false;

	return;
}

Vector3* Camera::TransformVector(Quaternion *pOrientation, Vector3 *pAxis) {
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