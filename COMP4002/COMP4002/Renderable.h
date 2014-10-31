#include <vector>
#include <glut.h>		   // The GL Utility Toolkit (GLUT) Header
#include <glew.h>
#include <math.h>
#include "mathlib.h"

struct Vertex {
	float x, y, z, w;

	Vertex() {}

	Vertex(float _x, float _y, float _z, float _w) {
		x = _x; y = _y; z = _z; w = _w;
	}
};

struct Color {
	float r, g, b, a;

	Color() {}

	Color(float _r, float _g, float _b, float _a) {
		r = _r; g = _g; b = _b; a = _a;
	}
};

GLuint mvpMatrixLoc;
GLuint vertexLoc;
GLuint colorLoc;

class Renderable {
public:
	GLuint vao, vbo, vio;
	Vector3 position = Vector3(0, 0, 0);
	Quaternion orientation = Quaternion();
	std::vector<Renderable*> children = std::vector<Renderable*>();

	void init_geometry(Vertex* vertices, Color* colors, int num_vertices, GLushort* indices, int num_indices) {
		GLuint buffers[3];

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		// Generate slots for the vertex and color buffers
		glGenBuffers(3, buffers);

		// bind buffer for vertices and copy data into buffer
		glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
		glBufferData(GL_ARRAY_BUFFER, num_vertices * sizeof(Vertex), vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(vertexLoc);
		glVertexAttribPointer(vertexLoc, 4, GL_FLOAT, 0, 0, 0);

		// bind buffer for colors and copy data into buffer
		glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
		glBufferData(GL_ARRAY_BUFFER, num_vertices * sizeof(Color), colors, GL_STATIC_DRAW);
		glEnableVertexAttribArray(colorLoc);
		glVertexAttribPointer(colorLoc, 4, GL_FLOAT, 0, 0, 0);
		vbo = buffers[1];

		// bind buffer for indices and copy data into buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[2]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_indices * sizeof(GLushort), indices, GL_STATIC_DRAW);
		vio = buffers[2];
	}

	Matrix4 matrix() {
		return orientation.toMatrix4()
			* Matrix4::translation(position.x, position.y, position.z);

	}

	void render(Matrix4 &parent) {
		auto self = matrix() * parent;

		glUniformMatrix4fv(mvpMatrixLoc, 1, false, (GLfloat*)&self);
		glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vio);

		render_self(self);

		render_children(self);
	}

	virtual void render_self(Matrix4 &self) = 0;

	void render_children(Matrix4 self) {
		for (auto it = children.begin(); it != children.end(); ++it) {
			(*it)->render(self);
		}
	}
};

class Sphere : public Renderable {
public:
	Sphere(float x, float y, float z, float radius) {
		position = Vector3(x, y, z);
		auto const rings = 10;
		auto const sectors = 10;
		auto const num_vertices = rings * sectors;
		auto R = 1. / (float)(rings - 1);
		auto S = 1. / (float)(sectors - 1);

		Vertex vertices[num_vertices];
		for (auto r = 0; r < rings; r++) {
			for (auto s = 0; s < sectors; s++) {
				auto y = sin(-Math::HALF_PI + Math::PI * r * R);
				auto x = cos(2 * Math::PI * s * S) * sin(Math::PI * r * R);
				auto z = sin(2 * Math::PI * s * S) * sin(Math::PI * r * R);

				vertices[r*sectors + s] = Vertex(x * radius, y * radius, z * radius, 1);
			}
		}

		std::vector<GLushort> indices;
		indices.resize(rings * sectors * 4);
		auto i = indices.begin();
		for (auto r = 0; r < rings - 1; r++) for (auto s = 0; s < sectors - 1; s++) {
			*i++ = r * sectors + s;
			*i++ = r * sectors + (s + 1);
			*i++ = (r + 1) * sectors + (s + 1);
			*i++ = (r + 1) * sectors + s;
		}

		Color colors[num_vertices];

		for (auto i = 0; i < num_vertices; ++i) {
			colors[i] = Color(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, 1);
		}

		init_geometry(vertices, colors, num_vertices, &indices[0], indices.size());
	}

	void render_self(Matrix4 &self) {
		// Draw the quads
		glDrawElements(
			GL_QUADS,            // mode
			9 * 9 * 4,		     // count
			GL_UNSIGNED_SHORT,   // type
			(void*)0             // element array buffer offset
			);
	}
};

class Box : public Renderable {
public:
	Box(float x, float y, float z, float width, float height, float depth) {
		position = Vector3(x, y, z);
		auto const num_vertices = 8;
		auto const num_indices = 24;

		auto min_x = -width / 2;
		auto max_x = width / 2;
		auto min_y = -height / 2;
		auto max_y = height / 2;
		auto min_z = -depth / 2;
		auto max_z = depth / 2;


		Vertex vertices[num_vertices] = {
			Vertex(min_x, min_y, min_z, 1),
			Vertex(min_x, min_y, max_z, 1),
			Vertex(min_x, max_y, min_z, 1),
			Vertex(min_x, max_y, max_z, 1),
			Vertex(max_x, min_y, min_z, 1),
			Vertex(max_x, min_y, max_z, 1),
			Vertex(max_x, max_y, min_z, 1),
			Vertex(max_x, max_y, max_z, 1),
		};

		GLushort indices[num_indices] = {
			0, 1, 3, 2,
			0, 1, 5, 4,
			0, 2, 6, 4,
			7, 6, 4, 5,
			7, 6, 2, 3,
			7, 3, 1, 5,
		};

		Color colors[num_vertices];

		for (auto i = 0; i < num_vertices; ++i) {
			colors[i] = Color(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, 1);
		}

		init_geometry(vertices, colors, num_vertices, indices, num_indices);
	}

	void render_self(Matrix4 &self) {
		// Draw the quads
		glDrawElements(
			GL_QUADS,            // mode
			24,					 // count
			GL_UNSIGNED_SHORT,   // type
			(void*)0             // element array buffer offset
			);
	}
};