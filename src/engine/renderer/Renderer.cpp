#include "Renderer.h"
#include "Shader.h"
#include "RenderJob.h"
#include "RenderInformation.h"
#include "..\scene\entity\component\Camera.h"

#include <vector>
#include <iostream>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <GL\glew.h>

Renderer::Renderer()
{
	_shader = new Shader("src/engine/renderer/shaders/pbr.vs", "src/engine/renderer/shaders/pbr.fs");

	_shader->use();

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
}

Renderer::~Renderer()
{
	delete _shader;
}

void Renderer::draw(RenderInformation *renderInformation)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (Camera *camera : *renderInformation->getCameras())
	{
		_shader->setUniformMat4("proj", camera->getProjectionMatrix());
		_shader->setUniformMat4("view", camera->getViewMatrix());

		for (RenderJob *renderJob : *renderInformation->getRenderJobs())
		{
			glm::mat4 modelMatrix = renderJob->getModelMatrix();
			_shader->setUniformMat4("model", modelMatrix);

			renderJob->draw();

			delete renderJob;
		}
	}

	delete renderInformation;
}

