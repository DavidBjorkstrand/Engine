#include "Mesh.h"
#include "../../SceneParser.h"
#include "../../../renderer/RenderJob.h"
#include <GL/glew.h>

Mesh::Mesh(vector<Vertex> *vertices, vector<GLuint> *indices)
{
	initBuffers(vertices, indices);
}

Mesh::~Mesh()
{

}

void Mesh::accept(SceneParser *parser) 
{
	parser->visit(this);
}

RenderJob *Mesh::getRenderJob(glm::mat4 modelMatrix)
{
	return new RenderJob(modelMatrix, _VAO, _nIndices);
}

void Mesh::initBuffers(vector<Vertex> *vertices, vector<GLuint> *indices)
{
	glGenVertexArrays(1, &_VAO);
	glGenBuffers(1, &_VBO);
	glGenBuffers(1, &_EBO);

	glBindVertexArray(_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, _VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices->size()*sizeof(Vertex), &vertices->at(0), GL_STATIC_DRAW);

	_nIndices = indices->size();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _nIndices*sizeof(GLuint), &indices->at(0), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, texCoords));

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	delete vertices;
	delete indices; 

}

Mesh *Mesh::createPlane()
{
	Vertex topLeft;
	Vertex topRight;
	Vertex bottomLeft;
	Vertex bottomRight;
	vector<Vertex> *vertices = new vector<Vertex>(); 
	vector<GLuint> *indices = new vector<GLuint>();

	bottomLeft.position.x = -0.5f;
	bottomLeft.position.y = -0.5f;
	bottomLeft.position.z = 0.0f;
	bottomLeft.normal.x = 0;
	bottomLeft.normal.y = 0;
	bottomLeft.normal.z = 1;
	bottomLeft.texCoords.s = 0.0f;
	bottomLeft.texCoords.t = 0.0f;
	vertices->push_back(bottomLeft);

	bottomRight.position.x = 0.5f;
	bottomRight.position.y = -0.5f;
	bottomRight.position.z = 0.0f;
	bottomRight.normal.x = 0;
	bottomRight.normal.y = 0;
	bottomRight.normal.z = 1;
	bottomRight.texCoords.s = 1.0f;
	bottomRight.texCoords.t = 0.0f;
	vertices->push_back(bottomRight);

	topRight.position.x = 0.5f;
	topRight.position.y = 0.5f;
	topRight.position.z = 0.0f;
	topRight.normal.x = 0;
	topRight.normal.y = 0;
	topRight.normal.z = 1;
	topRight.texCoords.s = 1.0f;
	topRight.texCoords.t = 1.0f;
	vertices->push_back(topRight);

	topLeft.position.x = -0.5f;
	topLeft.position.y = 0.5f;
	topLeft.position.z = 0.0f;
	topLeft.normal.x = 0;
	topLeft.normal.y = 0;
	topLeft.normal.z = 1;
	topLeft.texCoords.s = 0.0f;
	topLeft.texCoords.t = 1.0f;
	vertices->push_back(topLeft);

	
	indices->push_back(0);
	indices->push_back(1);
	indices->push_back(2);

	indices->push_back(2);
	indices->push_back(3);
	indices->push_back(0);

	return new Mesh(vertices, indices);
}