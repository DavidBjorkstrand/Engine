#include "RenderJob.h"

#include <glm/gtc/type_ptr.hpp>

RenderJob::RenderJob(glm::mat4 modelMatrix, GLuint VAO, GLuint nIndices)
{
	_modelMatrix = modelMatrix;
	_VAO = VAO;
	_nIndices = nIndices;
}

RenderJob::~RenderJob()
{

}

glm::mat4 RenderJob::getModelMatrix()
{
	return _modelMatrix;
}

void RenderJob::draw()
{
	glBindVertexArray(_VAO);
	glDrawElements(GL_TRIANGLES, _nIndices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}