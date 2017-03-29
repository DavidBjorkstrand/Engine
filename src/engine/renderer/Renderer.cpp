#include "Renderer.h"
#include "Shader.h"
#include "RenderJob.h"
#include "RenderInformation.h"
#include "..\scene\entity\component\Camera.h"
#include "..\scene\entity\Entity.h"
#include "..\scene\entity\Transform.h"
#include "Material.h"
#include "..\scene\entity\component\PointLight.h"
#include "..\interfaces\MaterialSystem.h"

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
	glEnable(GL_DEPTH_TEST);
}

Renderer::~Renderer()
{
	delete _shader;
}

void Renderer::draw(RenderInformation *renderInformation)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLuint nPointLights = renderInformation->getPointLights()->size();
	_shader->setUniform1i("nPointLights", nPointLights);

	for (int i = 0; i < nPointLights; i++)
	{
		glm::vec3 position = renderInformation->getPointLights()->at(i)->getEntity()->getTransform()->getWorldPosition();
		glm::vec3 color = renderInformation->getPointLights()->at(i)->getColor();

		_shader->setUniform3fv("pointLights[" + std::to_string(i) + "].position", position);
		_shader->setUniform3fv("pointLights[" + std::to_string(i) + "].color", color);
	}


	for (Camera *camera : *renderInformation->getCameras())
	{
		_shader->setUniformMat4("proj", camera->getProjectionMatrix());
		_shader->setUniformMat4("view", camera->getViewMatrix());
		_shader->setUniform3fv("viewPos", camera->getEntity()->getTransform()->getWorldPosition());

		for (RenderJob *renderJob : *renderInformation->getRenderJobs())
		{
			string materialName = renderJob->getMaterialName();
			Material *material = MaterialSystem::find(materialName);
			_shader->setUniform3fv("albedo", material->getAlbedo());
			_shader->setUniform1f("roughness", material->getRoughness());
			_shader->setUniform1f("metallic", material->getMetallic());

			glm::mat4 modelMatrix = renderJob->getModelMatrix();
			_shader->setUniformMat4("model", modelMatrix);

			renderJob->draw();

			delete renderJob;
		}
	}

	delete renderInformation;
}

