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

	void setOrientation(Quaternion);

	void setPosition(Vector3);
	void setPosition(float, float, float);

	Vector3 getPosition() const { return m_position; }
	Vector3 getLookAt() const { return m_lookatPosition; }
	Vector3 getUp() const { return up; }
	Quaternion getRotation() const { return m_quatOrientation; }

	Matrix4 getViewMatrix();

	void pitch(float angle) { applyRotate(angle, PITCH); }
	void roll(float angle)	{ applyRotate(angle, ROLL); }
	void yaw(float angle) { applyRotate(angle, YAW); }

	void move(float distance) { applyTranslation(distance, MOVE); }
	void strafe(float distance) { applyTranslation(distance, STRAFE); }
	void climb(float distance) { applyTranslation(distance, UPWARDS); }
	void scroll(float);
	void rotate(float);
	

	void setFollow(Vector3 *, float minHeight = 0.5f, float maxHeight = 2.5f);

	const Vector3 getAxisZ() const;
	const Vector3 getAxisY() const;
	const Vector3 getAxisX() const;
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

	void update();
	bool rotateZAxis(Quaternion*, float);
	bool rotateYAxis(Quaternion*, float);
	bool rotateXAxis(Quaternion*, float);
	static Vector3* transformVector(Quaternion*, Vector3*);
	void calculatePosition();

	void applyTranslation(float, eDir);
	void applyRotate(float, eOrient);
};

#endif