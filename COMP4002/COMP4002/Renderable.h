#pragma once

#include <stdio.h>
#include <vector>
#include <glut.h>		   // The GL Utility Toolkit (GLUT) Header
#include <glew.h>
#include <math.h>
#include "mathlib.h"
#include <SOIL.h>
#include "HeightMap.h"

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

GLuint shader1, shader2, shader3,shader4,shader5;

struct Texture2D {
	float u, v;
	
	Texture2D() {}

	Texture2D(float _u, float _v){
		u = _u; v = _v;
	}
};

struct Branch {
	float toprad;
	float botrad;
	float height;
	Matrix4 transform;

	Branch() {}

	Branch(float top, float bot, float hei, Matrix4 trans)
	{
		toprad = top;
		botrad = bot;
		height = hei;
		transform = trans;
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
	GLuint mvpMatrixLoc, vertexLoc, colorLoc,normalLoc, textUnitLoc, textCoordLoc, camPosLoc, modelMatrixLoc, instanceMatrixLoc;

	void init_geometry(Vertex* vertices, Color* colors,Vertex* normals, GLushort* indices, Texture2D *tCoords = 0, GLuint imageId = 0) {
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		// Generate slots for the vertex and color buffers
		glGenBuffers(5, buffers);

		// bind buffer for vertices and copy data into buffer
		vertexLoc = glGetAttribLocation(shader, "position");
		glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(vertexLoc);
		glVertexAttribPointer(vertexLoc, 4, GL_FLOAT, 0, 0, 0);

		normalLoc = glGetAttribLocation(shader, "normal");
		glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), normals, GL_STATIC_DRAW);
		glEnableVertexAttribArray(normalLoc);
		glVertexAttribPointer(normalLoc, 4, GL_FLOAT, 0, 0, 0);

		// bind buffer for colors and copy data into buffer
		if (isTextureShader)
		{
			textCoordLoc = glGetAttribLocation(shader, "textCoord");
			glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
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
			glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
			glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Color), colors, GL_STATIC_DRAW);
			glEnableVertexAttribArray(colorLoc);
			glVertexAttribPointer(colorLoc, 4, GL_FLOAT, 0, 0, 0);
		}

		mvpMatrixLoc = glGetUniformLocation(shader, "mvpMatrix");
		if (useInstance)
		{
			instanceMatrixLoc = glGetAttribLocation(shader, "instanceMatrix");

			glBindBuffer(GL_ARRAY_BUFFER, buffers[4]);

			for (unsigned int i = 0; i < 4; i++) {
				glEnableVertexAttribArray(instanceMatrixLoc + i);
				glVertexAttribPointer(instanceMatrixLoc + i, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix4),
					(const GLvoid*)(sizeof(GLfloat)* i * 4));
				glVertexAttribDivisor(instanceMatrixLoc + i, 1);
			}
		}
		else{
			
		}

		modelMatrixLoc = glGetUniformLocation(shader, "modelMatrix");
		camPosLoc = glGetUniformLocation(shader, "eyePosition"); 

		// bind buffer for indices and copy data into buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[3]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(GLushort), indices, GL_STATIC_DRAW);
		vio = buffers[3];
	}
	bool useMipmaps = false;
	bool useInstance = false;
	bool cullFace = true;
	int numIndices;
	int numVertices;
	GLuint buffers[5];
	void render_self(Matrix4 &parent, Matrix4 &self, std::vector<Matrix4> &modelMats, Vector3 camPosition){
		// Draw the quads
		if (!cullFace) glDisable(GL_CULL_FACE);

		glUseProgram(shader);
		glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vio);

		glUniform4f(camPosLoc, camPosition.x, camPosition.y, camPosition.z, 1);

		if (isTextureShader){
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureID);
			glUniform1i(textUnitLoc, 0);
		}
		if (useInstance){
			int numInstances = modelMats.size();

			glUniformMatrix4fv(mvpMatrixLoc, 1, true, (GLfloat*)&(parent));
			glUniformMatrix4fv(modelMatrixLoc, 1, true, (GLfloat*)&(self));

			glBindBuffer(GL_ARRAY_BUFFER, buffers[4]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(Matrix4)* numInstances, &modelMats[0], GL_DYNAMIC_DRAW);
			glBindVertexArray(vao);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vio);
			glDrawElementsInstanced(GL_TRIANGLES,
				numIndices,
				GL_UNSIGNED_SHORT,
				(void*)0,
				numInstances);
		} else {
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

	virtual void render(Matrix4 vpMatrix, Matrix4 &parent, std::vector<Matrix4> &modelMats, Vector3 camPosition) {
		auto self = parent * matrix();
		if (renderable) {
			renderable->render_self(vpMatrix, self, modelMats, camPosition);
		}

		render_children(vpMatrix, self, modelMats, camPosition);
	}

	void render_children(Matrix4 vpMatrix, Matrix4 self, std::vector<Matrix4> &modelMats,Vector3 camPosition) {
		for (auto it = children.begin(); it != children.end(); ++it) {
			(*it)->render(vpMatrix, self, modelMats, camPosition);
		}
	}

};

class Terrain :public Renderable
{
public:
	Terrain(GLuint shaderid, HeightMap h_map,bool useTexture = false, GLuint imageId = 0, bool mipmap = true, int subdivides = 1, float imageRatio = 1.0f)
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
		std::vector<Vertex> normals;
		normals.resize(numVertices);
		std::vector<Texture2D> tCoords;
		tCoords.resize(numVertices);

		int width, height, channels;
		unsigned char *ht_map;
		ht_map = SOIL_load_image("heightmap3.jpg", &width, &height, &channels, SOIL_LOAD_L);
		
		for (auto i = 0; i <= subdivides; i++) for (auto j = 0; j <= subdivides; j++)
		{
			float s = i * sub_x;
			float t = j * sub_z;
			vertices[i * (subdivides + 1) + j] = Vertex(s, h_map.lookup(s,t), t);
			tCoords[j + i * (subdivides + 1)] = Texture2D(((1.0f*i) / subdivides) * imageRatio, 1.0 - ((1.0f*j) / subdivides) * imageRatio);
			Vector3 edge1 = Vector3(s + sub_x, h_map.lookup(s + sub_x, t), t) - Vector3(s, h_map.lookup(s, t), t);
			edge1.normalize();
			Vector3 right1 = Vector3::cross(edge1, Vector3(0, 1, 0));
			Vector3 norm1 = Vector3::cross(right1, edge1);
			Vector3 edge2 = Vector3(s - sub_x, h_map.lookup(s - sub_x, t), t) - Vector3(s, h_map.lookup(s, t), t);
			edge2.normalize();
			Vector3 right2 = Vector3::cross(edge2, Vector3(0, 1, 0));
			Vector3 norm2 = Vector3::cross(right2, edge2);
			Vector3 edge3 = Vector3(s, h_map.lookup(s, t + sub_z), t + sub_z) - Vector3(s, h_map.lookup(s, t), t);
			edge3.normalize();
			Vector3 right3 = Vector3::cross(edge3, Vector3(0, 1, 0));
			Vector3 norm3 = Vector3::cross(right3, edge3);
			Vector3 edge4 = Vector3(s, h_map.lookup(s, t - sub_z), t - sub_z) - Vector3(s, h_map.lookup(s, t), t);
			edge4.normalize();
			Vector3 right4 = Vector3::cross(edge4, Vector3(0, 1, 0));
			Vector3 norm4 = Vector3::cross(right4, edge4);
			Vector3 norm = (norm1 + norm2 + norm3 + norm4) / 4;
			colors[j + i * (subdivides + 1)] = Color(0.3, 0.5, 0, 1);
			normals[j + i * (subdivides + 1)] = Vertex(norm.x, norm.y, norm.z, 0);
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

		init_geometry(&vertices[0], &colors[0], &normals[0], &indices[0], &tCoords[0], imageId);
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
		std::vector<Vertex> normals;
		normals.resize(numVertices);
		std::vector<Texture2D> tCoords;
		tCoords.resize(numVertices);

		int width2, height2, channels;
		unsigned char *ht_map;

		for (auto i = 0; i <= subdivides; i++) for (auto j = 0; j <= subdivides; j++)
		{
			vertices[i * (subdivides + 1) + j] = Vertex(min_x + i * sub_x,min_y + j*sub_y, 0, 1);
			tCoords[j + i *(subdivides + 1)] = Texture2D(((1.0f*i) / subdivides) * imageRatio, 1.0-((1.0f*j) / subdivides) * imageRatio);
			colors[j + i *(subdivides + 1)] = Color(0.3, 0.5, 0, 1);
			normals[j + i *(subdivides + 1)] = Vertex(0, 0, 1, 1);
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

		init_geometry(&vertices[0], &colors[0],&normals[0], &indices[0], &tCoords[0], imageId);
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
		Vertex* normals = new Vertex[numVertices];
		for (auto s = 0; s < half; s++) {
			auto x = cos(2 * Math::PI * s * S);
			auto z = sin(2 * Math::PI * s * S);

			vertices[s]        = Vertex(x * topradius, length, z * topradius, 1);
			vertices[half + s] = Vertex(x * botradius, 0, z * botradius, 1);
			normals[s] = Vertex(x, 0, z, 1);
			normals[half + s] = Vertex(x, 0, z, 1);
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

		init_geometry(vertices, colors, normals, &indices[0], tCoords, imageId);
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
		std::vector<Vertex> normals;
		normals.resize(numVertices);

		for (auto i = 0; i < modelMatrices.size(); i++)
		{
			vertices[i * 4] = Vertex(multMatVec(modelMatrices[i], Vector4(min_x, min_y, 0,1)));
			vertices[i * 4 + 1] = Vertex(multMatVec(modelMatrices[i],Vector4(max_x, min_y, 0, 1)));
			vertices[i * 4 + 2] = Vertex(multMatVec(modelMatrices[i],Vector4(min_x, max_y, 0, 1)));
			vertices[i * 4 + 3] = Vertex(multMatVec(modelMatrices[i],Vector4(max_x, max_y, 0, 1)));
			
			normals[i * 4] = Vertex(multMatVec(modelMatrices[i], Vector4(0, 0, 1, 0)));
			normals[i * 4 + 1] = Vertex(multMatVec(modelMatrices[i], Vector4(0, 0, 1, 0)));
			normals[i * 4 + 2] = Vertex(multMatVec(modelMatrices[i], Vector4(0, 0, 1, 0)));
			normals[i * 4 + 3] = Vertex(multMatVec(modelMatrices[i], Vector4(0, 0, 1, 0)));

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

		init_geometry(&vertices[0], &colors[0], &normals[0], &indices[0], &tCoords[0], imageId);
	}
};

class SuperBranch: public Renderable
{
public:
	SuperBranch(GLuint shaderId, int sectors, GLuint imageId, std::vector<Branch> &branchs, bool mipmap = false, bool useInst = false)
	{
		shader = shaderId;
		isTextureShader = true;
		useMipmaps = mipmap;
		useInstance = useInst;

		int vertsPerBranch = (sectors + 1) * 2;
		numVertices = vertsPerBranch * branchs.size();
		auto half = vertsPerBranch / 2;
		auto S = 1. / (float)(sectors);

		numIndices = ((sectors - 2) * 3 * 2 + sectors * 6) * branchs.size();

		std::vector<Vertex> vertices;
		vertices.resize(numVertices);
		std::vector<Color> colors;
		colors.resize(numVertices);
		std::vector<Texture2D> tCoords;
		tCoords.resize(numVertices);
		std::vector<Vertex> normals;
		normals.resize(numVertices);
		std::vector<GLushort> indices;
		indices.resize(numIndices);
		auto k = indices.begin();


		for (auto i = 0; i < branchs.size(); i++)
		{
			for (auto s = 0; s < half; s++) {
				auto x = cos(2 * Math::PI * s * S);
				auto z = sin(2 * Math::PI * s * S);

				vertices[vertsPerBranch * i + s] = Vertex(multMatVec(branchs[i].transform, Vector4(x * branchs[i].toprad, branchs[i].height, z * branchs[i].toprad, 1)));
				vertices[vertsPerBranch * i + half + s] = Vertex(multMatVec(branchs[i].transform, Vector4(x * branchs[i].botrad, 0, z * branchs[i].botrad, 1)));
				normals[vertsPerBranch * i + s] = Vertex(multMatVec(branchs[i].transform, Vector4(x, 0, z, 0)));
				normals[vertsPerBranch * i + half + s] = Vertex(multMatVec(branchs[i].transform, Vector4(x, 0, z, 0)));
				tCoords[vertsPerBranch * i + s].u = S*s;
				tCoords[vertsPerBranch * i + s].v = 0;
				tCoords[vertsPerBranch * i + s + half].u = S*s;
				tCoords[vertsPerBranch * i + s + half].v = 1;
			}

			for (auto s = 1; s < sectors - 1; s++) {//top
				*k++ = 0 + vertsPerBranch * i;
				*k++ = s + 1 + vertsPerBranch* i;
				*k++ = s + vertsPerBranch* i;
			}
			for (auto s = 1; s < sectors - 1; s++) {//bottom
				*k++ = half + vertsPerBranch* i;
				*k++ = half + s + vertsPerBranch* i;
				*k++ = half + s + 1 + vertsPerBranch* i;
			}
			for (auto s = 0; s < sectors; s++) {//side
				*k++ = s + vertsPerBranch* i;
				*k++ = s + 1 + vertsPerBranch* i;
				*k++ = half + s + vertsPerBranch* i;
				*k++ = half + s + vertsPerBranch* i;
				*k++ = s + 1 + vertsPerBranch* i;
				*k++ = half + s + 1 + vertsPerBranch* i;
			}
		}

		init_geometry(&vertices[0], &colors[0], &normals[0], &indices[0], &tCoords[0], imageId);
	}
};

class TreeNaive: public Entity {
public:
	std::vector<Renderable*> renderables;
	TreeNaive(Vector3 _pos, GLuint shaderid, bool useTexture, GLuint barkimage, GLuint leafimage) : Entity(_pos, 0) {
		float base_width = 15;
		float width_shrink_rate = 0.5;
		float width_to_length_rate = 20;
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
	std::vector<Branch> branchs;
	SuperLeaf *superleaf;
	SuperBranch *superbranch;

	TreeLSystem(Vector3 pos, GLuint shaderid, bool useTexture, GLuint barkImage, GLuint leafImage) 
			: Entity(pos, 0) {
		leaf_img = leafImage;
		bark_img = barkImage;
		shader = shaderid;
		texture = useTexture;
		Matrix4 stack = Matrix4::IDENTITY;
		//branchs.push_back(Branch(4, 4 * width_rate, height_rate * std::pow(2, max_depth), stack));
		recurse(0, 4, 0, 0, 0, max_depth, stack);
		superleaf = new SuperLeaf(shader5, 8, 12, leafImage, leafModels, true, true);
		children.push_back(new Entity(Vector3(0, 0, 0), superleaf));
		superbranch = new SuperBranch(shaderid, 10, barkImage, branchs, true, true);
		children.push_back(new Entity(Vector3(0, 0, 0), superbranch));
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
		else
		{
			Branch branch(new_width, width, height, stack);
			branchs.push_back(branch);
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
		//if (parent && !leaf) parent->children.push_back(entity);
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