#ifndef _MODEL_H_
#define _MODEL_H_

#include <windows.h>	   // Standard header for MS Windows applications

#include <GL/gl.h>		   // Open Graphics Library (OpenGL) header
#include <glut.h>	   // The GL Utility Toolkit (GLUT) Header

#include "mathlib.h"

using namespace std;

typedef struct {
	float r;
	float g;
	float b;
	float a;
} ColorType;

inline void setColValues(ColorType* ct, float _r,float _g, float _b, float _a){
	ct->r = _r;
	ct->g = _g;
	ct->b = _b;
	ct->a = _a;
}

inline void setDisplayColor(ColorType* ct){
	glColor4f(ct->r, ct->g, ct->b, ct->a);
}

class Model{

public:
	 Model();
	 Model(const Model&);
	 ~Model();

	 void Display();
	 void Initialize();
	 void Shutdown();

protected:
	Vector3* vertices;
	ColorType* colors;
	int m_vertexCount;

};

#endif