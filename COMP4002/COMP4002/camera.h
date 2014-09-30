////////////////////////////////////////////////////////////////////////////////
// Filename: cameraclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _CAMERACLASS_H_
#define _CAMERACLASS_H_


//////////////
// INCLUDES //
//////////////

#include <iostream>
#include <glut.h>
#include <mathlib.h>


#define _USE_MATH_DEFINES
#include <math.h>
#include <string.h>
//#include "state.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: CameraClass
////////////////////////////////////////////////////////////////////////////////
class Camera {
public:
	Camera();
	Camera(Vector3);
	Camera(const Camera&);
	~Camera();

	void SetOrientation(Quaternion);
	void SetLookAt(Vector3, Vector3, Vector3);

	void SetPosition(Vector3);
	void SetPosition(float, float, float);

	Vector3 GetPosition() const { return m_position; }
	Vector3 GetLookAt() const { return m_lookatPosition; }
	Vector3 GetUp() const { return up; }
	Quaternion GetRotation() const { return m_quatOrientation; }

	void GetViewMatrix(Matrix4&);

	void Pitch(float angle) { ApplyRotate(angle, PITCH); }
	void Roll(float angle)	{ ApplyRotate(angle, ROLL); }
	void Yaw(float angle) { ApplyRotate(angle, YAW); }

	void Move(float distance) { ApplyTranslation(distance, MOVE); }
	void Strafe(float distance) { ApplyTranslation(distance, STRAFE); }
	void Up(float distance) { ApplyTranslation(distance, UPWARDS); }
	void Scroll(float);
	void Rotate(float);
	

	void setFollow(Vector3 *, float minHeight = 0.5f, float maxHeight = 2.5f);

	const Vector3 GetAxisZ() const;
	const Vector3 GetAxisY() const;
	const Vector3 GetAxisX() const;
	bool isFirstPerson() { return firstPerson; }
	void toggleFirstPerson() { firstPerson = !firstPerson; }
	void lookUpDown(float);

	void display();

private:
	bool firstPerson;
	Vector3 m_position;
	Quaternion	m_quatOrientation;
	Matrix4 m_viewMatrix;
	Vector3 m_lookatPosition;

	Vector3 *follow;
	Vector3 up;

	bool upToDate;

	enum eDir { MOVE, STRAFE, UPWARDS };
	enum eOrient { PITCH, ROLL, YAW };

	void Update();
	bool RotateZAxis(Quaternion*, float);
	bool RotateYAxis(Quaternion*, float);
	bool RotateXAxis(Quaternion*, float);
	static Vector3* TransformVector(Quaternion*, Vector3*);
	void CalculatePosition();

	void ApplyTranslation(float, eDir);
	void ApplyRotate(float, eOrient);
};

#endif