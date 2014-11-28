#pragma once

#include "Renderable.h"

class Skybox {
public:
	GLuint vao, vbo, vio;
	GLuint shader;
	bool isTextureShader;
	GLuint textureID;
	GLuint mvpMatrixLoc, vertexLoc, textUnitLoc;

	Skybox(GLuint shaderid, GLuint imageId)
	{
		shader = shaderid;

		const int num_vertices = 24;
		const int num_indices = 36;

		Vertex vertices[num_vertices] = { Vertex(-1, -1, 1), Vertex(1, -1, 1), Vertex(-1, 1, 1), Vertex(1, 1, 1),  //front face  v0,v1,v2,v3
			Vertex(1, -1, -1), Vertex(-1, -1, -1), Vertex(1, 1, -1), Vertex(-1, 1, -1),   //back face   v4,v5,v6,v7
			Vertex(1, -1, 1), Vertex(1, -1, -1), Vertex(1, 1, 1), Vertex(1, 1, -1),   //right face  v1 v4 v3 v6
			Vertex(-1, -1, -1), Vertex(-1, -1, 1), Vertex(-1, 1, -1), Vertex(-1, 1, 1),   //left face   v5 v0 v7 v2
			Vertex(-1, -1, -1), Vertex(1, -1, -1), Vertex(-1, -1, 1), Vertex(1, -1, 1),  //bottom face v5 v4 v0 v1
			Vertex(-1, 1, 1), Vertex(1, 1, 1), Vertex(-1, 1, -1), Vertex(1, 1, -1) }; //top face    v2 v3 v7 v6
		
		GLushort indices[num_indices] { 0, 2, 1, 1, 2, 3,   //front face
			4, 6, 5, 5, 6, 7,   //back face
			8, 10, 9, 9, 10, 11,   //right face
			12, 14, 13, 13, 14, 15,   //left face
			16, 18, 17, 17, 18, 19,   //bottom face
			20, 22, 21, 21, 22, 23 }; //top face
		/*
		GLushort indices[num_indices] { 0, 1,2,2,1, 3,   //front face
			4, 5,6, 6,5, 7,   //back face
			8, 9,10, 10, 9, 11,   //right face
			12, 13, 14, 14, 13, 15,   //left face
			16, 17, 18, 18, 17, 19,   //bottom face
			20, 21, 22, 22, 21, 23 }; //top face
		*/
		init_geometry(vertices, num_vertices, indices, num_indices, imageId);
		
	}

	void init_geometry(Vertex* vertices, int num_vertices, GLushort* indices, int num_indices, GLuint imageId = 0) {
		GLuint buffers[3];

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		// Generate slots for the vertex and color buffers
		glGenBuffers(3, buffers);

		// bind buffer for vertices and copy data into buffer
		vertexLoc = glGetAttribLocation(shader, "position");
		glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
		glBufferData(GL_ARRAY_BUFFER, num_vertices * sizeof(Vertex), vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(vertexLoc);
		glVertexAttribPointer(vertexLoc, 4, GL_FLOAT, 0, 0, 0);


		// Typical Texture Generation Using Data From The Bitmap
		textureID = imageId;
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP);

		textUnitLoc = glGetUniformLocation(shader, "texCube");
		
		// bind buffer for indices and copy data into buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[2]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_indices * sizeof(GLushort), indices, GL_STATIC_DRAW);
		vio = buffers[2];

		mvpMatrixLoc = glGetUniformLocation(shader, "mvpMatrix");
	}
	
	void render_self(Matrix4 &self, Vector3 cameraPosition)
	{
		Matrix4 scale = Matrix4(10000 / 1.73f, 0, 0, 0,
			0, 10000/1.73f, 0, 0,
			0, 0, 10000/1.73f, 0,
			0, 0, 0, 1
		);
		Matrix4 trans = Matrix4(1, 0, 0, cameraPosition.x,
			0, 1, 0, cameraPosition.y,
			0, 0, 1, cameraPosition.z,
			0, 0, 0, 1);
		//glDisable(GL_DEPTH);
		glUseProgram(shader);
		glUniformMatrix4fv(mvpMatrixLoc, 1, true, (GLfloat*)&(self*trans*scale));
		
		glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vio);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
		glUniform1i(textUnitLoc, 0);

		glDrawElements(
			GL_TRIANGLES,            // mode
			36,		     // count
			GL_UNSIGNED_SHORT,   // type
			(void*)0             // element array buffer offset
			);
		//glEnable(GL_DEPTH);

	}
};