#include "Model.h"


Model::Model() {
	vertices = 0;
	colors = 0;
}

Model::Model(const Model& other) { }

Model::~Model() { }

void Model::Initialize() {

	m_vertexCount = 3;

	vertices = new Vector3[3];
	colors = new ColorType[3];

	vertices[0].set(-1.0f, -1.0f, 0.0f);
	setColValues(&colors[0], 1.0f, 0.0f, 0.0f, 1.0f);
	vertices[1].set(1.0f, -1.0f, 0.0f);
	setColValues(&colors[1], 0.0f, 0.0f, 1.0f, 1.0f);
	vertices[2].set(-1.0f, 1.0f, 0.0f);
	setColValues(&colors[2], 0.0f, 1.0f, 0.0f, 1.0f);

}

void Model::Shutdown() {
	if (vertices){
		delete[] vertices;
		vertices = 0;
	}
	if (colors){
		delete[] colors;
		colors = 0;
	}

	return;
}

void Model::Display(){

	for (int i = 0; i < m_vertexCount; i++)
	{
		setDisplayColor(&colors[i]);
		glVertex3f(vertices[i].x, vertices[i].y, vertices[i].z);
	}

}