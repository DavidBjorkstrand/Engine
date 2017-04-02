#include "engine/renderer/RenderSystem.h"

#include "engine/renderer/Shader.h"
#include "engine/renderer/Material.h"
#include "engine/renderer/Texture.h"
#include "engine/renderer/CubeMap.h"
#include "engine/scene/SkyBox.h"
#include "engine/scene/Scene.h"
#include "engine/scene/entity/Entity.h"
#include "engine/scene/entity/Transform.h"
#include "engine/scene/entity/component/Camera.h"
#include "engine/scene/entity/component/PointLight.h"
#include "engine/scene/entity/component/Mesh.h"
#include "engine/interface/Resources.h"
#include "engine/interface/Window.h"

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
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glEnable(GL_MULTISAMPLE);
}

RenderSystem::~RenderSystem()
{
	
}

void RenderSystem::setScene(Scene *scene)
{
	_scene = scene;
	createIrradianceMap();
	createPreFilterEnvMap();
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

			GLuint textureUnit = GL_TEXTURE0;
			if (shader->hasUniform("irradianceMap"))
			{
				shader->bindCubeMap(_irradianceMap, textureUnit, "irradianceMap");
				textureUnit++;
			}

			if (shader->hasUniform("preFilterEnvMap"))
			{
				shader->bindCubeMap(_preFilterEnvMap, textureUnit, "preFilterEnvMap");
				textureUnit++;
			}

			if (shader->hasUniform("brdfLUT"))
			{
				shader->bindTexture(_brdfLut, textureUnit, "brdfLUT");
				textureUnit++;
			}

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
				else if (type == "samplerCube" && name != "irradianceMap" && name != "preFilterEnvMap")
				{
					CubeMap *value = material->getCubeMap(name);

					if (value)
					{
						shader->bindCubeMap(value, textureUnit, name);
						textureUnit++;
					}
				}
				else if (type == "texture")
				{
					Texture *value = material->getTexture(name);

					if (value)
					{
						shader->bindTexture(value, textureUnit, name);
						textureUnit++;
					}
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

			for (GLuint i = GL_TEXTURE0; i < textureUnit; ++i)
			{
				glActiveTexture(i);
				glBindTexture(GL_TEXTURE_2D, 0);
				glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
			}
		}
	}
}

void RenderSystem::createIrradianceMap()
{
	GLuint captureFBO;
	GLuint captureRBO;

	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	glGenRenderbuffers(1, &captureRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glGenFramebuffers(1, &captureFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
	
	_irradianceMap = new CubeMap(32, false);
	Mesh *mesh = _scene->getSkyBox()->getMesh();
	GLuint VAO = mesh->getVAO();
	GLuint nIndices = mesh->getNIndices();
	Material *material = mesh->getMaterial();
	Shader *shader = Resources::findShader("irradiance");

	glDepthFunc(mesh->getDepthFunc());

	shader->use();
	shader->bindCubeMap(material->getCubeMap("skybox"), GL_TEXTURE0, "skybox");
	shader->setUniformMat4("proj", captureProjection);

	glViewport(0, 0, 32, 32);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (GLuint i = 0; i < 6; i++)
	{
		shader->setUniformMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, _irradianceMap->getTextureID(), 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLE_STRIP, nIndices, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, Window::getWindowWidth(), Window::getWindowHeight());
}

void RenderSystem::createPreFilterEnvMap()
{
	GLuint captureFBO;
	GLuint captureRBO;

	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	glGenRenderbuffers(1, &captureRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glGenFramebuffers(1, &captureFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

	_preFilterEnvMap = new CubeMap(128, true);

	Mesh *mesh = _scene->getSkyBox()->getMesh();
	GLuint VAO = mesh->getVAO();
	GLuint nIndices = mesh->getNIndices();
	Material *material = mesh->getMaterial();
	Shader *shader = Resources::findShader("preFilterEnvMap");

	shader->use();
	shader->bindCubeMap(material->getCubeMap("skybox"), GL_TEXTURE0, "skybox");
	shader->setUniformMat4("proj", captureProjection);

	GLuint maxMipLevels = 5;
	for (GLuint mip = 0; mip < maxMipLevels; ++mip)
	{
		unsigned int mipWidth = 128 * std::pow(0.5, mip);
		unsigned int mipHeight = 128 * std::pow(0.5, mip);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		shader->setUniform1f("roughness", roughness);

		for (GLuint i = 0; i < 6; ++i)
		{
			shader->setUniformMat4("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, _preFilterEnvMap->getTextureID(), mip);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLE_STRIP, nIndices, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	Shader *brdfShader = Resources::findShader("brdf");
	Mesh *plane = Mesh::createPlane();
	GLuint planeVAO = plane->getVAO();
	GLuint planeNIndices = plane->getNIndices();
	_brdfLut = new Texture();

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _brdfLut->getTextureID(), 0);

	glViewport(0, 0, 512, 512);

	brdfShader->use();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(planeVAO);
	glDrawElements(GL_TRIANGLE_STRIP, planeNIndices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, Window::getWindowWidth(), Window::getWindowHeight());
}

