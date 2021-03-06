#include "engine/renderer/RenderSystem.h"

#include "engine/renderer/Shader.h"
#include "engine/renderer/Material.h"
#include "engine/renderer/Texture.h"
#include "engine/renderer/CubeMap.h"
#include "engine/renderer/TextureGenerator.h"
#include "engine/renderer/FluidRenderer.h"
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
	_screenAlignedQuad = Mesh::createPlane();
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_POINT_SPRITE);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_LINE_SMOOTH);
	glLineWidth(5.0f);

	_fluidRenderer = new FluidRenderer();

	_sceneGenerator = new TextureGenerator(Window::getWindowWidth(), Window::getWindowHeight(), GL_RGB32F, GL_DEPTH_COMPONENT, GL_NEAREST);
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
	GLuint currentShader = -1;
	string currentMaterial = "";
	GLuint textureUnit = GL_TEXTURE3;

	_scene->traverse();

	vector<Camera *> *cameras = _scene->getCameras();
	vector<PointLight *> *pointLights = _scene->getPointLights();
	vector<RenderCommand> *renderCommands = _scene->getRenderCommands();
	vector<RenderCommand> fluidCommands;
	GLuint nPointLights = pointLights->size();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (Camera *camera : *cameras)
	{
		_sceneGenerator->bind(true);
		for (RenderCommand renderCommand : *renderCommands)
		{
			if (renderCommand.mesh->getMaterial()->getShaderName() == "fluid")
			{
				fluidCommands.push_back(renderCommand);
				continue;
			}
			Mesh *mesh = renderCommand.mesh;
			Material *material = mesh->getMaterial();
			Shader *shader = material->getShader();
			GLboolean newShader = false;

			// If new shader
			if (shader->getID() != currentShader)
			{
				currentShader = shader->getID();
				newShader = true;

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

				if (shader->hasUniform("irradianceMap"))
				{
					shader->bindCubeMap(_irradianceMap, GL_TEXTURE0, "irradianceMap");
				}

				if (shader->hasUniform("preFilterEnvMap"))
				{
					shader->bindCubeMap(_preFilterEnvMap, GL_TEXTURE1, "preFilterEnvMap");
				}

				if (shader->hasUniform("brdfLUT"))
				{
					shader->bindTexture(_brdfLut, GL_TEXTURE2, "brdfLUT");
				}
			}

			// If new material or new shader
			if (material->getName() != currentMaterial || newShader)
			{
				// If new material and old shader unbind textures.
				if (material->getName() != currentMaterial && !newShader)
				{
					for (GLuint i = GL_TEXTURE3; i < textureUnit; ++i)
					{
						glActiveTexture(i);
						glBindTexture(GL_TEXTURE_2D, 0);
						glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
					}

					textureUnit = GL_TEXTURE3;
				}

				//If new material
				if (material->getName() != currentMaterial)
				{
					if (material->getBlend())
					{
						glEnable(GL_BLEND);
					} 
					else {
						glDisable(GL_BLEND);
					}

					glBlendFunc(material->getBlendSrc(), material->getBlendDst());
				}

				currentMaterial = material->getName();

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
			}
			

			glDepthFunc(renderCommand.mesh->getDepthFunc());

			glm::mat4 modelMatrix = renderCommand.modelMatrix;
			shader->setUniformMat4("model", modelMatrix);

			GLuint VAO = mesh->getVAO();
			GLuint nIndices = mesh->getNIndices();

			glBindVertexArray(VAO);
			glDrawElements(mesh->getDrawMode(), nIndices, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
		_sceneGenerator->unbind();
		glDepthFunc(GL_LESS);

		_fluidRenderer->draw(camera, &fluidCommands);

		Shader *shader;
		shader = Resources::findShader("particlePbr");
		shader->use();
		shader->bindCubeMap(_irradianceMap, GL_TEXTURE0, "irradianceMap");
		shader->bindCubeMap(_preFilterEnvMap, GL_TEXTURE1, "preFilterEnvMap");
		shader->bindTexture(_brdfLut, GL_TEXTURE2, "brdfLUT");
		shader->bindTexture(_sceneGenerator->getColorTarget(), GL_TEXTURE3, "sceneColor");
		shader->bindTexture(_sceneGenerator->getDepthTarget(), GL_TEXTURE4, "sceneDepth");
		shader->bindTexture(_fluidRenderer->getDepthMap(), GL_TEXTURE5, "particleDepth");
		shader->bindTexture(_fluidRenderer->getThicknessMap(), GL_TEXTURE6, "particleThickness");
		shader->bindTexture(_fluidRenderer->getNoiseMap(), GL_TEXTURE7, "noise");
		shader->setUniformMat4("proj", camera->getProjectionMatrix());
		shader->setUniformMat4("view", camera->getViewMatrix());
		shader->setUniform3fv("viewPos", camera->getEntity()->getTransform()->getWorldPosition());

		GLuint VAO = _screenAlignedQuad->getVAO();
		GLuint nIndices = _screenAlignedQuad->getNIndices();

		glBindVertexArray(VAO);
		glDrawElements(_screenAlignedQuad->getDrawMode(), nIndices, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glDisable(GL_BLEND);
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
	_brdfLut = new Texture(512, 512, GL_RG16F, GL_CLAMP_TO_EDGE, GL_LINEAR);

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

