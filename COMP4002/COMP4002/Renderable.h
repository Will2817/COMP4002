#pragma once

#include <stdio.h>
#include <vector>
#include <glut.h>		   // The GL Utility Toolkit (GLUT) Header
#include <glew.h>
#include <math.h>
#include "mathlib.h"
#include <SOIL.h>

struct Vertex {
	float x, y, z, w;

	Vertex() {}

	Vertex(float _x, float _y, float _z){
		x = _x; y = _y; z = _z; w = 1;
	}

	Vertex(float _x, float _y, float _z, float _w) {
		x = _x; y = _y; z = _z; w = _w;
	}

	Vertex(Vector3 _vec, float _w)
	{
		x = _vec.x; y = _vec.y; z = _vec.z;  w = _w;
	}

	Vertex(Vector4 _vec)
	{
		x = _vec.x; y = _vec.y; z = _vec.z;  w = _vec.w;
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

GLuint shader1, shader2, shader3,shader4;

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

	void init_geometry(Vertex* vertices, Color* colors, GLushort* indices, Texture2D *tCoords = 0, GLuint imageId = 0) {
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		// Generate slots for the vertex and color buffers
		glGenBuffers(4, buffers);

		// bind buffer for vertices and copy data into buffer
		vertexLoc = glGetAttribLocation(shader, "position");
		glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(vertexLoc);
		glVertexAttribPointer(vertexLoc, 4, GL_FLOAT, 0, 0, 0);

		// bind buffer for colors and copy data into buffer
		if (isTextureShader)
		{
			textCoordLoc = glGetAttribLocation(shader, "textCoord");
			glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
			glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Texture2D), tCoords, GL_STATIC_DRAW);
			glEnableVertexAttribArray(textCoordLoc);
			glVertexAttribPointer(textCoordLoc, 2, GL_FLOAT, 0, 0, 0);

			
			// Typical Texture Generation Using Data From The Bitmap
			textureID = imageId;
			glBindTexture(GL_TEXTURE_2D, textureID);


			float aniso = 0.0f;
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);

			if (useMipmaps)
			{
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}
			else{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}

			textUnitLoc = glGetUniformLocation(shader, "texUnit");
		}
		else
		{
			colorLoc = glGetAttribLocation(shader, "color");
			glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
			glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Color), colors, GL_STATIC_DRAW);
			glEnableVertexAttribArray(colorLoc);
			glVertexAttribPointer(colorLoc, 4, GL_FLOAT, 0, 0, 0);
			vbo = buffers[1];
		}

		if (useInstance)
		{
			mvpMatrixLoc = glGetAttribLocation(shader, "mvpMatrix");

			glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);

			for (unsigned int i = 0; i < 4; i++) {
				glEnableVertexAttribArray(mvpMatrixLoc + i);
				glVertexAttribPointer(mvpMatrixLoc + i, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix4),
					(const GLvoid*)(sizeof(GLfloat)* i * 4));
				glVertexAttribDivisor(mvpMatrixLoc + i, 1);
			}
		}
		else{
			mvpMatrixLoc = glGetUniformLocation(shader, "mvpMatrix");
		}

		// bind buffer for indices and copy data into buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[2]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(GLushort), indices, GL_STATIC_DRAW);
		vio = buffers[2];
	}
	bool useMipmaps = false;
	bool useInstance = false;
	bool cullFace = true;
	int numIndices;
	int numVertices;
	GLuint buffers[4];
	void render_self(Matrix4 &parent, Matrix4 &self, std::vector<Matrix4> &modelMats){
		// Draw the quads
		if (!cullFace) glDisable(GL_CULL_FACE);

		glUseProgram(shader);
		glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vio);

		if (isTextureShader)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureID);
			glUniform1i(textUnitLoc, 0);
		}
		if (useInstance)
		{
			/*int numInstances = modelMats.size();
			std::vector<Matrix4> mvpMats;
			mvpMats.resize(numInstances);
			int index = 0;
			for (auto it = modelMats.begin(); it != modelMats.end(); ++it)
			{
				mvpMats[index++] = parent * (*it) * self;
			}*/
			int numInstances = 100;
			Matrix4 mvpMats[100];
			for (int i = 0; i < 10; i++) for (int j = 0; j < 10; j++)
			{
				mvpMats[i * 10 + j] = parent * modelMats[i*10+j] * self;
			}

			glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(Matrix4)* numInstances, &mvpMats[0], GL_DYNAMIC_DRAW);
			glBindVertexArray(vao);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vio);
			glDrawElementsInstanced(GL_TRIANGLES,
				numIndices,
				GL_UNSIGNED_SHORT,
				(void*)0,
				numInstances);
		}
		else {
			glUniformMatrix4fv(mvpMatrixLoc, 1, true, (GLfloat*)&(parent*self));
			glDrawElements(
				GL_TRIANGLES,            // mode
				numIndices,					 // count
				GL_UNSIGNED_SHORT,   // type
				(void*)0             // element array buffer offset
				);
		}
		if (!cullFace) glEnable(GL_CULL_FACE);
	}
};

class Entity {
public:
	Vector3 position = Vector3(0, 0, 0);
	Quaternion orientation = Quaternion::IDENTITY;
	Renderable* renderable;
	std::vector<Entity*> children = std::vector<Entity*>();
	Vector3 scale = Vector3(1, 1, 1);

	Entity(Vector3 _pos, Renderable* renderable) {
		position = _pos;
		this->renderable = renderable;
	}

	Matrix4 matrix() {
		Matrix4 scaleMatrix = Matrix4(scale.x, 0, 0, 0, 0, scale.y, 0, 0, 0, 0, scale.z, 0, 0, 0, 0, 1);
		return Matrix4::translation(position.x, position.y, position.z) * orientation.toMatrix4() * scaleMatrix;
	}

	void setScale(Vector3 _scale) { scale = _scale; }

	virtual void render(Matrix4 vpMatrix, Matrix4 &parent, std::vector<Matrix4> &modelMats) {
		auto self = parent * matrix();
		if (renderable) {
			renderable->render_self(vpMatrix, self, modelMats);
		}

		render_children(vpMatrix, self, modelMats);
	}

	void render_children(Matrix4 vpMatrix, Matrix4 self, std::vector<Matrix4> &modelMats) {
		for (auto it = children.begin(); it != children.end(); ++it) {
			(*it)->render(vpMatrix, self, modelMats);
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
class Terrain :public Renderable
{
public:
	Terrain(GLuint shaderid, bool useTexture = false, GLuint imageId = 0, bool mipmap = true, int subdivides = 1, float imageRatio = 1.0f)
	{
		shader = shaderid;
		isTextureShader = useTexture;
		useMipmaps = mipmap;

		auto sub_x = 1.0f / subdivides;
		auto sub_z = 1.0f / subdivides;

		numVertices = pow((subdivides + 1), 2);
		numIndices = 6 * pow(subdivides, 2);

		std::vector<Vertex> vertices;
		vertices.resize(numVertices);
		std::vector<Color> colors;
		colors.resize(numVertices);
		std::vector<Texture2D> tCoords;
		tCoords.resize(numVertices);

		int width, height, channels;
		unsigned char *ht_map;
		ht_map = SOIL_load_image("heightmap3.jpg", &width, &height, &channels, SOIL_LOAD_L);
		
		for (auto i = 0; i <= subdivides; i++) for (auto j = 0; j <= subdivides; j++)
		{
			int s = i * sub_x  * width;
			int t = j*sub_z * height;
			vertices[i * (subdivides + 1) + j] = Vertex(i * sub_x, ht_map[s * width + t]/255.0f, j*sub_z, 1);
			tCoords[j + i *(subdivides + 1)] = Texture2D(((1.0f*i) / subdivides) * imageRatio, 1.0 - ((1.0f*j) / subdivides) * imageRatio);
			colors[j + i *(subdivides + 1)] = Color(0.3, 0.5, 0, 1);
		}

		int index = 0;
		std::vector<GLushort> indices;
		indices.resize(numIndices);
		for (auto i = 0; i < subdivides; i += 1) for (auto j = 0; j < subdivides; j += 1)
		{
			indices[index++] = j + i * (subdivides + 1);
			indices[index++] = j + i * (subdivides + 1) + 1;
			indices[index++] = j + (i + 1) * (subdivides + 1);
			indices[index++] = j + (i + 1) * (subdivides + 1);
			indices[index++] = j + i * (subdivides + 1) + 1;
			indices[index++] = j + (i + 1) * (subdivides + 1) + 1;

		}

		init_geometry(&vertices[0], &colors[0], &indices[0], &tCoords[0], imageId);
	}
};

class Plane :public Renderable
{
public:
	Plane(float width, float height, GLuint shaderid, bool useTexture = false, GLuint imageId = 0, bool mipmap = true, int subdivides = 1, float imageRatio = 1.0f, bool useInst = false) {
		shader = shaderid;
		isTextureShader = useTexture;
		useMipmaps = mipmap;
		useInstance = useInst;

		auto min_x = -width / 2;
		auto max_x = width / 2;
		auto min_y = 0;
		auto max_y = height;

		auto sub_x = width / subdivides;
		auto sub_y = height / subdivides;

		numVertices = pow((subdivides + 1), 2);
		numIndices = 6 * pow(subdivides, 2);
		 
		std::vector<Vertex> vertices;
		vertices.resize(numVertices);
		std::vector<Color> colors;
		colors.resize(numVertices);
		std::vector<Texture2D> tCoords;
		tCoords.resize(numVertices);

		int width2, height2, channels;
		unsigned char *ht_map;

		for (auto i = 0; i <= subdivides; i++) for (auto j = 0; j <= subdivides; j++)
		{
			vertices[i * (subdivides + 1) + j] = Vertex(min_x + i * sub_x,min_y + j*sub_y, 0, 1);
			tCoords[j + i *(subdivides + 1)] = Texture2D(((1.0f*i) / subdivides) * imageRatio, 1.0-((1.0f*j) / subdivides) * imageRatio);
			colors[j + i *(subdivides + 1)] = Color(0.3, 0.5, 0, 1);
		}

		int index = 0;
		std::vector<GLushort> indices;
		indices.resize(numIndices);
		for (auto i = 0; i < subdivides; i += 1) for (auto j = 0; j < subdivides; j += 1)
		{
			indices[index++] = j + i * (subdivides+1);
			indices[index++] = j + i * (subdivides + 1) + 1;
			indices[index++] = j + (i + 1) * (subdivides + 1);
			indices[index++] = j + (i + 1) * (subdivides + 1);
			indices[index++] = j + i * (subdivides + 1) + 1;
			indices[index++] = j + (i + 1) * (subdivides + 1) + 1;

		}

		init_geometry(&vertices[0], &colors[0], &indices[0], &tCoords[0], imageId);
	}
};

class Cylinder: public Renderable {
public:
	Cylinder(int sectors, float topradius, float botradius, float length, GLuint shaderid, bool useTexture=false, GLuint imageId = 0, bool useInst = false) {
		shader = shaderid;
		isTextureShader = useTexture;
		useInstance = useInst;
		useMipmaps = true;
		numVertices = (sectors+1) * 2;
		auto half = numVertices / 2;
		auto S = 1. / (float)(sectors);

		Vertex* vertices = new Vertex[numVertices];
		useMipmaps = true;
		for (auto s = 0; s < half; s++) {
			auto x = cos(2 * Math::PI * s * S);
			auto z = sin(2 * Math::PI * s * S);

			vertices[s]        = Vertex(x * topradius, length, z * topradius, 1);
			vertices[half + s] = Vertex(x * botradius, 0, z * botradius, 1);
		}


		std::vector<GLushort> indices;
		numIndices = (sectors - 2) * 3 * 2 + sectors * 6;
		indices.resize(numIndices);
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

		Color *colors = new Color[numVertices];

		for (auto i = 0; i < numVertices; ++i) {
			colors[i] = Color(0.5,0.3,0, 1);
		}

		Texture2D *tCoords = new Texture2D[numVertices];
		for (auto i = 0; i < half; i++){
			tCoords[i].u = S*i;
			tCoords[i].v = 0;
			tCoords[i + half].u = S*i;
			tCoords[i + half].v = 1;
		}

		init_geometry(vertices, colors, &indices[0], tCoords, imageId);
	}
};

class Leaf : public Plane {
public: 
	Leaf(GLuint shaderId, GLuint imageId, bool useInst = false) : Plane(8, 12, shaderId, true, imageId,true,1,1,useInst){
		cullFace = false;
	}
};

class SuperLeaf : public Renderable {
public:
	SuperLeaf(GLuint shaderId, int width, int height, GLuint imageId, std::vector<Matrix4> &modelMatrices,bool mipmap = false, bool useInst = false)
	{
		shader = shaderId;
		isTextureShader = true;
		useMipmaps = mipmap;
		useInstance = useInst;
		cullFace = false;

		auto min_x = -width / 2;
		auto max_x = width / 2;
		auto min_y = 0;
		auto max_y = height;

		numVertices = 4 * modelMatrices.size();
		numIndices = 6 * modelMatrices.size();

		std::vector<Vertex> vertices;
		vertices.resize(numVertices);
		std::vector<Color> colors;
		colors.resize(numVertices);
		std::vector<Texture2D> tCoords;
		tCoords.resize(numVertices);

		for (auto i = 0; i < modelMatrices.size(); i++)
		{
			vertices[i * 4] = Vertex(multMatVec(modelMatrices[i], Vector4(min_x, min_y, 0,1)));
			vertices[i * 4 + 1] = Vertex(multMatVec(modelMatrices[i],Vector4(max_x, min_y, 0, 1)));
			vertices[i * 4 + 2] = Vertex(multMatVec(modelMatrices[i],Vector4(min_x, max_y, 0, 1)));
			vertices[i * 4 + 3] = Vertex(multMatVec(modelMatrices[i],Vector4(max_x, max_y, 0, 1)));
			tCoords[i*4] = Texture2D(0,1);
			tCoords[i*4+1] = Texture2D(1, 1);
			tCoords[i*4+2] = Texture2D(0, 0);
			tCoords[i*4+3] = Texture2D(1, 0);
		}

		int index = 0;
		std::vector<GLushort> indices;
		indices.resize(numIndices);
		for (auto i = 0; i < modelMatrices.size(); i++)
		{
			indices[index++] = i * 4;
			indices[index++] = i * 4 + 1;
			indices[index++] = i * 4 + 2;
			indices[index++] = i * 4 + 2;
			indices[index++] = i * 4 + 1;
			indices[index++] = i * 4 + 3;
		}

		init_geometry(&vertices[0], &colors[0], &indices[0], &tCoords[0], imageId);
	}
};

class TreeNaive: public Entity {
public:
	std::vector<Renderable*> renderables;
	TreeNaive(Vector3 _pos, GLuint shaderid, bool useTexture, GLuint barkimage, GLuint leafimage) : Entity(_pos, 0) {
		float base_width = 2;
		float width_shrink_rate = 0.8;
		float width_to_length_rate = 6;
		float numsplit = 3;
		float mingirth = 1; 
		float curvature = 30;
		children.push_back(
			makeRecursive(Vector3(0,0,0), base_width, width_shrink_rate, width_to_length_rate, numsplit, mingirth, curvature, shaderid, useTexture, 0, barkimage, leafimage)
		);
	}
	
	Entity* makeRecursive(Vector3 pos, float base_width, float width_shrink_rate, float width_to_length_rate, float numsplit, float mingirth, float curvature, GLuint shaderid, bool useTexture, int depth, GLuint barkimage, GLuint leafimage) {
		auto height = sqrt(base_width) * width_to_length_rate;
		auto next_width = base_width * width_shrink_rate;
		if (depth == renderables.size()) {
			if (base_width > mingirth) {
				renderables.push_back(new Cylinder(10, next_width, base_width, height, shaderid, useTexture, barkimage));
			}
			else {
				renderables.push_back(new Leaf(shaderid,leafimage));
			}
		}
		auto part = new Entity(pos, renderables[depth]);
		if (base_width > mingirth) {
			float offset = randf()/2;
			for (auto i = 0; i < numsplit; ++i) {
				part->children.push_back(makeRecursive(Vector3(0, height*(1.0 - offset), 0), next_width, width_shrink_rate, width_to_length_rate, numsplit + 0.25, mingirth, curvature * 1.1, shaderid, useTexture, depth + 1, barkimage, leafimage));
				part->children.back()->orientation.fromHeadPitchRoll(360 / numsplit * i, 0, curvature + randf()*(curvature/2));
				offset = 0;
			}
		} else {
			for (auto i = 0; i < numsplit - 2; ++i) {
				float offset = randf();
				part->children.push_back(new Entity(Vector3(0, height*(1.0 - offset), 0), renderables[depth]));
				part->children.back()->orientation.fromHeadPitchRoll(360 * i / numsplit, 0, 30 + randf() * 30);
			}
		}
		return part;
	}
};

class TreeLSystem: public Entity {
public:
	std::vector<Renderable*> renderables;
	int max_depth = 7;
	int min_depth = 2;

	float tilt_rate = 25;
	float height_rate = 0.5;
	float width_rate = 0.5;

	GLuint shader;
	bool texture;
	GLuint leaf_img = 0;
	GLuint bark_img = 0;
	std::vector<Matrix4> leafModels;
	SuperLeaf *superleaf;

	TreeLSystem(Vector3 pos, GLuint shaderid, bool useTexture, GLuint barkImage, GLuint leafImage) 
			: Entity(pos, 0) {
		leaf_img = leafImage;
		bark_img = barkImage;
		shader = shaderid;
		texture = useTexture;
		Matrix4 stack = Matrix4::IDENTITY;
		children.push_back(recurse(0, 4, 0, 0, 0, max_depth, stack));
		superleaf = new SuperLeaf(shaderid, 8, 12, leafImage, leafModels, true, true);
		children.push_back(new Entity(Vector3(0, 0, 0), superleaf));
	}

	Entity* recurse(Entity* parent, float width, float v_offset, float tilt, float angle, int depth, Matrix4 stack) {
		
		if (depth < min_depth) return parent; 

		float height = height_rate * std::pow(2, depth);
		float new_width = width * width_rate;
		
		auto renderable = getRenderable(depth, height * 2, width, new_width);
		Entity* entity;
		Entity* root;

		root = entity = makeEntity(v_offset, tilt, angle, parent, renderable, depth == min_depth);
		stack *= entity->matrix();
		if (depth == min_depth)
		{
			leafModels.push_back(stack);
		}
		tilt -= tilt_rate;
		{
			{
				recurse(entity, new_width, height, tilt, 0, depth - 1, stack);
			}
			tilt += tilt_rate;
			recurse(entity, new_width, height, tilt, 120, depth - 1, stack);
		} tilt -= tilt_rate;
		tilt += tilt_rate;

		auto new_new_width = new_width / 2;
		{
			tilt += tilt_rate;
			recurse(entity, new_new_width, height * 2, tilt, 70, depth - 1, stack);
		} tilt -= tilt_rate;
		tilt -= tilt_rate;
		recurse(entity, new_new_width, height * 2, tilt, 160, depth - 1, stack);
		 
		return root;
	}

	Entity* makeEntity(float height, float tilt, float angle, Entity* parent, Renderable* renderable, bool leaf) {
		auto entity = new Entity(Vector3(0, height, 0), renderable);
		if (leaf) {
			entity->orientation.fromHeadPitchRoll(tilt, tilt, angle);
		} else {
			entity->orientation.fromHeadPitchRoll(angle, 0, tilt);
			
		}
		if (parent && !leaf) parent->children.push_back(entity);
		return entity;
	}

	Renderable* getRenderable(int depth, float height, float width, float new_width) {
		int index = max_depth - depth;
		if (renderables.size() <= index) {
			if (depth == min_depth) {
				renderables.push_back(new Leaf(shader,leaf_img,true));
			} else {
				renderables.push_back(new Cylinder(10, new_width, width, height, shader, texture, bark_img,true));
			}
		}
		return renderables[index];
	}
};

/*
class BushLSystem : public Entity {
public:
	std::vector<Renderable*> renderables;
	int max_depth = 4;
	GLuint shader;
	bool texture;
	GLuint leaf_img = 0;
	GLuint bark_img = 0;

	BushLSystem(Vector3 pos, GLuint shaderid, bool useTexture, GLuint barkImage, GLuint leafImage)
		: Entity(pos, 0) {
		leaf_img = leafImage;
		bark_img = barkImage;
		shader = shaderid;
		texture = useTexture;
		children.push_back(recurse(0, 1, 0, 0, 0, max_depth));
		children.push_back(recurse(0, 1, 0, 0, 0, max_depth));
		children.push_back(recurse(0, 1, 0, 0, 0, max_depth));

		float amount = 0;
		for (auto it = children.begin(); it != children.end(); ++it) {
			amount += 120;
			(*it)->orientation.fromHeadPitchRoll(amount, 0, 45);
		}

	}

	Entity* recurse(Entity* parent, float width, float v_offset, float tilt, float angle, int depth) {

		if (depth <= 0) return parent;

		float tilt_rate = 25;
		float height_rate = 0.3;
		float width_rate = 0.5;

		float height = height_rate * std::pow(2, depth);
		float new_width = width * width_rate;

		auto renderable = getRenderable(depth, height * 2, width, new_width);
		Entity* entity;
		Entity* root;

		root = entity = makeEntity(v_offset, tilt, angle, parent, renderable, depth == 1);
		tilt -= tilt_rate;
		{
			{
				recurse(entity, new_width, height, tilt, 0, depth - 1);
			}
			tilt += tilt_rate;
			recurse(entity, new_width, height, tilt, 120, depth - 1);
		} tilt -= tilt_rate;
		tilt += tilt_rate;

		auto new_new_width = new_width / 2;
		{
			tilt += tilt_rate;
			recurse(entity, new_new_width, height * 2, tilt, 70, depth - 1);
		} tilt -= tilt_rate;
		tilt -= tilt_rate;
		recurse(entity, new_new_width, height * 2, tilt, 160, depth - 1);

		return root;
	}

	Entity* makeEntity(float height, float tilt, float angle, Entity* parent, Renderable* renderable, bool leaf) {
		auto entity = new Entity(Vector3(0, height, 0), renderable);
		if (leaf) {
			entity->orientation.fromHeadPitchRoll(tilt, tilt, angle);
		}
		else {
			entity->orientation.fromHeadPitchRoll(angle, 0, tilt);
		}

		if (parent) parent->children.push_back(entity);
		return entity;
	}

	Renderable* getRenderable(int depth, float height, float width, float new_width) {
		int index = max_depth - depth;
		if (renderables.size() <= index) {
			if (index == max_depth - 1) {
				
				renderables.push_back(new Leaf(leaf_img));
			}
			else {
				renderables.push_back(new Cylinder(10, new_width, width, height, shader, texture, bark_img));
			}
		}
		return renderables[index];
	}
};
*/
