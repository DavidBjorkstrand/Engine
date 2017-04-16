#include "engine/scene/entity/component/Mesh.h"

#include "engine/scene/Scene.h"
#include "engine/scene/entity/Entity.h"
#include "engine/scene/entity/Transform.h"
#include "engine/interface/Resources.h"
#include "engine/renderer/RenderSystem.h"

#include <string>

#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>

Mesh::Mesh(vector<Vertex> *vertices, vector<GLuint> *indices, string materialName)
{
	_vertices = vertices;
	_indices = indices;
	_material = Resources::findMaterial(materialName);

	_depthFunc = GL_LESS;

	_usage = GL_STATIC_DRAW;

	initBuffers(vertices, indices);
}

Mesh::Mesh(vector<Vertex> *vertices, vector<GLuint> *indices, string materialName, GLenum usage)
{
	_material = Resources::findMaterial(materialName);

	_depthFunc = GL_LESS;

	_usage = usage;

	initBuffers(vertices, indices);
}

Mesh::~Mesh()
{
	delete _vertices;
	delete _indices;
}

void Mesh::accept(Scene *scene) 
{
	scene->visit(this);
}

void Mesh::setMaterial(string materialName)
{
	_material = Resources::findMaterial(materialName);
}

void Mesh::setMaterial(Material *material)
{
	_material = material;
}

void Mesh::setDepthFunc(GLenum depthFunc)
{
	_depthFunc = depthFunc;
}

Material *Mesh::getMaterial()
{
	return _material;
}

GLuint Mesh::getVAO()
{
	return _VAO;
}

GLuint Mesh::getNIndices()
{
	return _indices->size();
}

GLenum Mesh::getDepthFunc()
{
	return _depthFunc;
}

void Mesh::updateBuffers(vector<Vertex> *vertices, vector<GLuint> *indices)
{
	_vertices = vertices;
	_indices = indices;

	glBindVertexArray(_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, _VBO);
	glBufferData(GL_ARRAY_BUFFER, _vertices->size() * sizeof(Vertex), &vertices->at(0), _usage);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices->size() * sizeof(GLuint), &indices->at(0), _usage);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, texCoords));

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void Mesh::initBuffers(vector<Vertex> *vertices, vector<GLuint> *indices)
{
	glGenVertexArrays(1, &_VAO);
	glGenBuffers(1, &_VBO);
	glGenBuffers(1, &_EBO);

	updateBuffers(vertices, indices);
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
	indices->push_back(3);
	indices->push_back(1);
	indices->push_back(2);

	return new Mesh(vertices, indices, "default");
}

Mesh *Mesh::createSphere()
{
	vector<Vertex> *vertices = new vector<Vertex>();
	vector<GLuint> *indices = new vector<GLuint>();

	const unsigned int X_SEGMENTS = 64;
	const unsigned int Y_SEGMENTS = 64;
	const float PI = 3.14159265359;

	for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
	{
		for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
		{
			Vertex vertex;

			float xSegment = (float)x / (float)X_SEGMENTS;
			float ySegment = (float)y / (float)Y_SEGMENTS;
			float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
			float yPos = std::cos(ySegment * PI);
			float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

			vertex.position = glm::vec3(xPos/2, yPos/2, zPos/2);
			vertex.normal = glm::normalize(glm::vec3(xPos, yPos, zPos));
			vertex.texCoords = glm::vec2(xSegment, ySegment);

			vertices->push_back(vertex);
		}
	}

	bool oddRow = false;
	for (int y = 0; y < Y_SEGMENTS; ++y)
	{
		if (!oddRow)
		{
			for (int x = 0; x <= X_SEGMENTS; ++x)
			{
				indices->push_back(y       * (X_SEGMENTS + 1) + x);
				indices->push_back((y + 1) * (X_SEGMENTS + 1) + x);
			}
		}
		else
		{
			for (int x = X_SEGMENTS; x >= 0; --x)
			{
				indices->push_back((y + 1) * (X_SEGMENTS + 1) + x);
				indices->push_back(y       * (X_SEGMENTS + 1) + x);
			}
		}
		oddRow = !oddRow;
	}

	return new Mesh(vertices, indices, "default");

}

Mesh *Mesh::createCube()
{
	vector<Vertex> *vertices = new vector<Vertex>();
	vector<GLuint> *indices = new vector<GLuint>();

	Vertex one;
	one.position = glm::vec3(0.5f, 0.5f, -0.5f);
	one.normal = glm::normalize(glm::vec3(1.0f, 1.0f, -1.0f));
	one.texCoords = glm::vec2(1.0f, 1.0f);

	Vertex two;
	two.position = glm::vec3(-0.5f, 0.5f, -0.5f);
	two.normal = glm::normalize(glm::vec3(-1.0f, 1.0f, -1.0f));
	two.texCoords = glm::vec2(0.0f, 1.0f);

	Vertex three;
	three.position = glm::vec3(0.5f, -0.5f, -0.5f);
	three.normal = glm::normalize(glm::vec3(1.0f, -1.0f, -1.0f));
	three.texCoords = glm::vec2(1.0f, 0.0f);

	Vertex four;
	four.position = glm::vec3(-0.5f, -0.5f, -0.5f);
	four.normal = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
	four.texCoords = glm::vec2(0.0f, 0.0f);

	Vertex five;
	five.position = glm::vec3(0.5f, 0.5f, 0.5f);
	five.normal = glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f));
	five.texCoords = glm::vec2(1.0f, 1.0f);

	Vertex six;
	six.position = glm::vec3(-0.5f, 0.5f, 0.5f);
	six.normal = glm::normalize(glm::vec3(-1.0f, 1.0f, 1.0f));
	six.texCoords = glm::vec2(0.0f, 1.0f);

	Vertex seven;
	seven.position = glm::vec3(-0.5f, -0.5f, 0.5f);
	seven.normal = glm::normalize(glm::vec3(-1.0f, -1.0f, 1.0f));
	seven.texCoords = glm::vec2(0.0f, 0.0f);

	Vertex eight;
	eight.position = glm::vec3(0.5f, -0.5f, 0.5f);
	eight.normal = glm::normalize(glm::vec3(1.0f, -1.0f, 1.0f));
	eight.texCoords = glm::vec2(1.0f, 0.0f);

	//Bottom
	four.texCoords = glm::vec2(0.0f, 1.0f);
	three.texCoords = glm::vec2(1.0f, 1.0f);
	seven.texCoords = glm::vec2(0.0f, 0.0f);
	eight.texCoords = glm::vec2(1.0f, 0.0f);

	vertices->push_back(four);
	vertices->push_back(three);
	vertices->push_back(seven);
	vertices->push_back(eight);

	//Right half of front
	five.texCoords = glm::vec2(1.0f, 1.0f);

	vertices->push_back(five);

	//Right
	three.texCoords = glm::vec2(0.0f, 0.0f);
	one.texCoords = glm::vec2(0.0f, 1.0f);
	
	vertices->push_back(three);
	vertices->push_back(one);

	//Back
	four.texCoords = glm::vec2(1.0f, 0.0f);
	two.texCoords = glm::vec2(1.0f, 1.0f);

	vertices->push_back(four);
	vertices->push_back(two);

	//Left
	seven.texCoords = glm::vec2(0.0f, 0.0f);
	six.texCoords = glm::vec2(0.0f, 1.0f);

	vertices->push_back(seven);
	vertices->push_back(six);

	//Left half front
	five.texCoords = glm::vec2(1.0f, 1.0f);

	vertices->push_back(five);

	//Top
	two.texCoords = glm::vec2(0.0f, 0.0f);
	one.texCoords = glm::vec2(1.0f, 0.0f);

	vertices->push_back(two);
	vertices->push_back(one);

	indices->push_back(0);
	indices->push_back(1);
	indices->push_back(2);
	indices->push_back(3);
	indices->push_back(4);
	indices->push_back(5);
	indices->push_back(6);
	indices->push_back(7);
	indices->push_back(8);
	indices->push_back(9);
	indices->push_back(10);
	indices->push_back(11);
	indices->push_back(12);
	indices->push_back(13);


	return new Mesh(vertices, indices, "default");

}