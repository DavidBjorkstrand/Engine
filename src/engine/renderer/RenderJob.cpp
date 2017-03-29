#include "RenderJob.h"
#include "..\scene\entity\component\Component.h"

#include <string>

#include <glm/gtc/type_ptr.hpp>

RenderJob::RenderJob(glm::mat4 modelMatrix, GLuint VAO, GLuint nIndices, string materialName)
{
	_modelMatrix = modelMatrix;
	_VAO = VAO;
	_nIndices = nIndices;
	_materialName = materialName;
}

RenderJob::~RenderJob()
{

}

glm::mat4 RenderJob::getModelMatrix()
{
	return _modelMatrix;
}

string RenderJob::getMaterialName()
{
	return _materialName;
}

void RenderJob::draw()
{
	glBindVertexArray(_VAO);
	glDrawElements(GL_TRIANGLE_STRIP, _nIndices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}