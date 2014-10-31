#pragma once
// test1.cpp : Defines the entry point for the console application.
//

//
//  //
//  Created by nuss on 2014-07-29.
//  Copyright (c) 2014 Dorn Nussbaum . All rights reserved.
//
//  	This program is for education purposes


/*
	LICENSE AGREEMENT 

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

	Copyright and License agreement must remain if software is used.

    This program is distributed	AS IS in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/


/************************************************************************/



#include <windows.h>


#include "glew.h"

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include "glut.h"
#endif

#include "gl/gl.h"

class Shader
{
public:
	Shader(void);
	~Shader(void);
	char * readCode(char * fileName);
	int createShaderObj(char* fileName , int shaderType, GLuint *shaderid);	
	
	// functions creates a shader program.  The two shader programs (vertex and fragment) were already compiled.
	GLint ceateShaderProgram(GLint vertShaderid, GLint fragShaderid, GLuint *shaderProgId);

private:
	GLuint vertShaderid;
	GLuint fragShaderid;
public:
	GLuint shaderProgramid;
	// creates a shader program from files vsFileName and fsFileName
	int createShaderProgram(char * vsFileName, char * fsFileName, GLuint *shaderProgramid);
	int shaderStatus(void);
};

