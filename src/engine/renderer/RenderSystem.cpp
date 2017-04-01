#include "engine/renderer/RenderSystem.h"

#include "engine/renderer/Shader.h"
#include "engine/renderer/Material.h"
#include "engine/renderer/Texture.h"
#include "engine/scene/Scene.h"
#include "engine/scene/entity/Entity.h"
#include "engine/scene/entity/Transform.h"
#include "engine/scene/entity/component/Camera.h"
#include "engine/scene/entity/component/PointLight.h"
#include "engine/scene/entity/component/Mesh.h"
#include "engine/interface/Resources.h"

#include <vector>
#include <map>
#include <iostream>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <GL\glew.h>

RenderSystem::RenderSystem()
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
}

RenderSystem::~RenderSystem()
{
	
}

void RenderSystem::setScene(Scene *scene)
{
	_scene = scene;
}

void RenderSystem::draw()
{
	_scene->traverse();

	vector<Camera *> *cameras = _scene->getCameras();
	vector<PointLight *> *pointLights = _scene->getPointLights();
	vector<RenderCommand> *renderCommands = _scene->getRenderCommands();
	GLuint nPointLights = pointLights->size();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (Camera *camera : *cameras)
	{
		for (RenderCommand renderCommand : *renderCommands)
		{
			Mesh *mesh = renderCommand.mesh;
			Material *material = mesh->getMaterial();
			Shader *shader = material->getShader();

			shader->use();

			/*
			* TODO: Only set these if there is a new shader.
			*/
			shader->setUniform1i("nPointLights", nPointLights);
			for (GLuint i = 0; i < nPointLights; i++)
			{
				glm::vec3 position = (*pointLights)[i]->getEntity()->getTransform()->getWorldPosition();
				glm::vec3 color = (*pointLights)[i]->getColor();

				shader->setUniform3fv("pointLights[" + std::to_string(i) + "].position", position);
				shader->setUniform3fv("pointLights[" + std::to_string(i) + "].color", color);
			}

			shader->setUniformMat4("proj", camera->getProjectionMatrix());
			shader->setUniformMat4("view", camera->getViewMatrix());
			shader->setUniform3fv("viewPos", camera->getEntity()->getTransform()->getWorldPosition());

			map<string, string> *uniforms = shader->getUniforms();
			for (map<string, string>::iterator it = uniforms->begin(); it != uniforms->end(); ++it)
			{
				string name = it->first;
				string type = it->second;

				if (type == "vec3")
				{
					glm::vec3 value = material->getVec3(name);

					shader->setUniform3fv(name, value);
				}
				else if (type == "float")
				{
					float value = material->getFloat(name);

					shader->setUniform1f(name, value);
				}
				else if (type == "samplerCube")
				{
					CubeMap *value = material->getCubeMap(name);

					shader->bindCubeMap(value, GL_TEXTURE0, name);
				}
			}

			glDepthFunc(renderCommand.mesh->getDepthFunc());

			glm::mat4 modelMatrix = renderCommand.modelMatrix;
			shader->setUniformMat4("model", modelMatrix);

			GLuint VAO = mesh->getVAO();
			GLuint nIndices = mesh->getNIndices();

			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLE_STRIP, nIndices, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
	}
}

