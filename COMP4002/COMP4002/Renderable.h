#include <vector>
#include <glut.h>		   // The GL Utility Toolkit (GLUT) Header
#include <glew.h>
#include <math.h>
#include "mathlib.h"
#include <SOIL.h>

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

float randf() {
	return ((float)rand()) / (float) RAND_MAX;
}

GLuint shader1, shader2;

struct Texture2D {
	float u, v;
	
	Texture2D() {}

	Texture2D(float _u, float _v){
		u = _u; v = _v;
	}
};

int bark_img_width, bark_img_height, leaf_image_width, leaf_image_height;
unsigned char* bark_img;
unsigned char* leaf_img;

class Renderable {
public:
	GLuint vao, vbo, vio;
	GLuint shader;
	bool isTextureShader;
	GLuint textureID;
	GLuint mvpMatrixLoc, vertexLoc, colorLoc, textUnitLoc, textCoordLoc;

	void init_geometry(Vertex* vertices, Color* colors, int num_vertices, GLushort* indices, int num_indices, Texture2D *tCoords = 0, char *imagename = 0) {
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

		// bind buffer for colors and copy data into buffer
		if (isTextureShader)
		{
			textCoordLoc = glGetAttribLocation(shader, "textCoord");
			glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
			glBufferData(GL_ARRAY_BUFFER, num_vertices * sizeof(Texture2D), tCoords, GL_STATIC_DRAW);
			glEnableVertexAttribArray(textCoordLoc);
			glVertexAttribPointer(textCoordLoc, 2, GL_FLOAT, 0, 0, 0);

			glGenTextures(1, &textureID);

			textureID = SOIL_load_OGL_texture
				(
				imagename,
				SOIL_LOAD_AUTO,
				SOIL_CREATE_NEW_ID,
				SOIL_FLAG_MIPMAPS
				);


			// Typical Texture Generation Using Data From The Bitmap
			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);


			/*
			glBindTexture(GL_TEXTURE_2D, textureID);
			
			if (useMipmaps)
			{
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				gluBuild2DMipmaps(GL_TEXTURE_2D, 3, img_width, img_height, GL_RGBA, GL_UNSIGNED_BYTE, img);
			}
			else{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
			}*/

			textUnitLoc = glGetAttribLocation(shader, "texUnit");
		}
		else
		{
			colorLoc = glGetAttribLocation(shader, "color");
			glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
			glBufferData(GL_ARRAY_BUFFER, num_vertices * sizeof(Color), colors, GL_STATIC_DRAW);
			glEnableVertexAttribArray(colorLoc);
			glVertexAttribPointer(colorLoc, 4, GL_FLOAT, 0, 0, 0);
			vbo = buffers[1];
		}

		// bind buffer for indices and copy data into buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[2]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_indices * sizeof(GLushort), indices, GL_STATIC_DRAW);
		vio = buffers[2];

		mvpMatrixLoc = glGetUniformLocation(shader, "mvpMatrix");
	}
	bool useMipmaps = false;
	virtual void render_self(Matrix4 &self) = 0;
};

class Entity {
public:
	Vector3 position = Vector3(0, 0, 0);
	Quaternion orientation = Quaternion::IDENTITY;
	Renderable* renderable;
	std::vector<Entity*> children = std::vector<Entity*>();

	Entity(float x, float y, float z, Renderable* renderable) {
		position = Vector3(x, y, z);
		this->renderable = renderable;
	}

	Matrix4 matrix() {
		return Matrix4::translation(position.x, position.y, position.z) * orientation.toMatrix4();
	}

	void render(Matrix4 &parent) {
		auto self = parent * matrix();
		glUseProgram(renderable->shader);
		glUniformMatrix4fv(renderable->mvpMatrixLoc, 1, true, (GLfloat*)&self);
		glBindVertexArray(renderable->vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderable->vio);

		renderable->render_self(self);

		render_children(self);
	}

	void render_children(Matrix4 self) {
		for (auto it = children.begin(); it != children.end(); ++it) {
			(*it)->render(self);
		}
	}

};

/*
class Sphere : public Entity {
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
			colors[i] = Color(.3, .5, 0, 1);
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
*/
class Plane :public Renderable
{
public:
	Plane(float width, float height, float depth, GLuint shaderid, bool useTexture = false) {
		shader = shaderid;
		isTextureShader = useTexture;
		auto const num_vertices = 8;
		auto const num_indices = 24;

		auto min_x = -width / 2;
		auto max_x = width / 2;
		auto min_y = -height / 2;
		auto max_y = height / 2;

		Vertex vertices[num_vertices] = {
			Vertex(min_x, min_y, 0, 1),
			Vertex(max_x, min_y, 0, 1),
			Vertex(max_x, max_y, 0, 1),
			Vertex(min_x, max_y, 0, 1)
		};

		GLushort indices[num_indices] = {
			0, 1, 2 ,3, 3,2,1,0
		};

		Color colors[num_vertices];

		for (auto i = 0; i < num_vertices; ++i) {
			colors[i] = Color(0.3,0.5,0,1);
		}

		Texture2D tCoords[8] = {
			Texture2D(0,0),
			Texture2D(1,0),
			Texture2D(1,1),
			Texture2D(0,1),
			Texture2D(0, 1),
			Texture2D(1, 1),
			Texture2D(1, 0),
			Texture2D(0, 0)
		};

		init_geometry(vertices, colors, num_vertices, indices, num_indices, tCoords,"templeaf.png");
	}

	void render_self(Matrix4 &self) {
		// Draw the quads
		
		if (isTextureShader)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureID);
			glUniform1i(textUnitLoc, 0);
		}

		glDrawElements(
			GL_QUADS,            // mode
			8,					 // count
			GL_UNSIGNED_SHORT,   // type
			(void*)0             // element array buffer offset
			);
	}
};

class Cylinder: public Renderable {
public:
	int numindices;
	Cylinder(int sectors, float topradius, float botradius, float length, GLuint shaderid, bool useTexture=false) {
		shader = shaderid;
		isTextureShader = useTexture;
		auto const num_vertices = (sectors+1) * 2;
		auto half = num_vertices / 2;
		auto S = 1. / (float)(sectors);

		Vertex* vertices = new Vertex[num_vertices];
		useMipmaps = true;
		for (auto s = 0; s < half; s++) {
			auto x = cos(2 * Math::PI * s * S);
			auto z = sin(2 * Math::PI * s * S);

			vertices[s]        = Vertex(x * topradius, length, z * topradius, 1);
			vertices[half + s] = Vertex(x * botradius, 0, z * botradius, 1);
		}


		std::vector<GLushort> indices;
		numindices = (sectors - 2) * 3 * 2 + sectors * 6;
		indices.resize(numindices);
		auto i = indices.begin();
		for (auto s = 1; s < sectors - 1; s++) {//top
			*i++ = 0;
			*i++ = s + 1;
			*i++ = s;
		}
		for (auto s = 1; s < sectors - 1; s++) {//bottom
			*i++ = half;
			*i++ = half + s;
			*i++ = half + s + 1;
		}
		for (auto s = 0; s < sectors; s++) {//side
			*i++ = s;
			*i++ = s + 1;
			*i++ = half + s;
			*i++ = half + s;
			*i++ = s + 1;
			*i++ = half + s + 1;
		}


		Color *colors = new Color[num_vertices];

		for (auto i = 0; i < num_vertices; ++i) {
			colors[i] = Color(0.5,0.3,0, 1);
		}

		Texture2D *tCoords = new Texture2D[num_vertices];
		for (auto i = 0; i < half; i++){
			tCoords[i].u = S*i;
			tCoords[i].v = 0;
			tCoords[i + half].u = S*i;
			tCoords[i + half].v = 1;
		}

		init_geometry(vertices, colors, num_vertices, &indices[0], indices.size(), tCoords, "nature_bark.png");
	}

	void render_self(Matrix4 &self) {
		if (isTextureShader)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureID);
			glUniform1i(textUnitLoc, 0);
		}

		// Draw the quads
		glDrawElements(
			GL_TRIANGLES,            // mode
			numindices,		     // count
			GL_UNSIGNED_SHORT,   // type
			(void*)0             // element array buffer offset
			);
	}
};

class Leaf : public Plane {
public: 
	Leaf() : Plane (8, 12, 0, shader2, true){}
};

class TreeNaive {
public:
	std::vector<Renderable*> renderables;
	Entity* root;
	TreeNaive(float x, float y, float z, float base_width, float width_shrink_rate, float width_to_length_rate, float numsplit, float mingirth, float curvature, GLuint shaderid, bool useTexture = false) {
		root = makeRecursive(x, y, z, base_width, width_shrink_rate, width_to_length_rate, numsplit, mingirth, curvature, shaderid, useTexture, 0);
	}
	
	Entity* makeRecursive(float x, float y, float z, float base_width, float width_shrink_rate, float width_to_length_rate, float numsplit, float mingirth, float curvature, GLuint shaderid, bool useTexture, int depth) {
		auto height = sqrt(base_width) * width_to_length_rate;
		auto next_width = base_width * width_shrink_rate;
		if (depth == renderables.size()) {
			if (base_width > mingirth) {
				renderables.push_back(new Cylinder(10, next_width, base_width, height, shaderid, useTexture));
			}
			else {
				renderables.push_back(new Leaf());
			}
		}
		auto part = new Entity(x, y, z, renderables[depth]);
		if (base_width > mingirth) {
			float offset = randf()/2;
			for (auto i = 0; i < numsplit; ++i) {
				part->children.push_back(makeRecursive(0, height*(1.0 - offset), 0, next_width, width_shrink_rate, width_to_length_rate, numsplit + 0.25, mingirth, curvature * 1.1, shaderid, useTexture, depth + 1));
				part->children.back()->orientation.fromHeadPitchRoll(360 / numsplit * i, 0, curvature + randf()*(curvature/2));
				offset = 0;
			}
		} else {
			for (auto i = 0; i < numsplit - 2; ++i) {
				float offset = randf();
				part->children.push_back(new Entity(0, height*(1.0 - offset), 0, renderables[depth]));
				part->children.back()->orientation.fromHeadPitchRoll(360 * i / numsplit, 0, 30 + randf() * 30);
			}
		}
		return part;
	}
};

