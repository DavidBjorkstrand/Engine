#include "engine/renderer/FluidRenderer.h"

#include "engine/renderer/RenderSystem.h"
#include "engine/renderer/TextureGenerator.h"
#include "engine/renderer/Shader.h"
#include "engine/renderer/Texture.h"
#include "engine/renderer/Material.h"
#include "engine/scene/entity/component/Camera.h"
#include "engine/scene/entity/component/Mesh.h"
#include "engine/interface/Window.h"

#include <vector>

#include <GL\glew.h>

FluidRenderer::FluidRenderer()
{
	_screenAlignedQuad = Mesh::createPlane();

	_depthShader = new Shader("src/engine/renderer/shaders/fluid/particle.vs", "src/engine/renderer/shaders/fluid/particleDepth.fs");
	_fluidDepthMapGenerator = new TextureGenerator(Window::getWindowWidth(), Window::getWindowHeight(), GL_RGB32F, GL_NONE, GL_NEAREST);

	_thicknessShader = new Shader("src/engine/renderer/shaders/fluid/particle.vs", "src/engine/renderer/shaders/fluid/thickness.fs");
	_fluidThicknessMapGenerator = new TextureGenerator(Window::getWindowWidth(), Window::getWindowHeight(), GL_RGB32F, GL_NONE, GL_NEAREST);

	_curvatureFlow = new Shader("src/engine/renderer/shaders/fluid/bilateralFilter.vs", "src/engine/renderer/shaders/fluid/curvatureFlow.fs");
	_smoother = new Shader("src/engine/renderer/shaders/fluid/bilateralFilter.vs", "src/engine/renderer/shaders/fluid/bilateralFilter.fs");
	_fluidDepthMapSmoother[0] = new TextureGenerator(Window::getWindowWidth(), Window::getWindowHeight(), GL_RGB32F, GL_NONE, GL_NEAREST);
	_fluidDepthMapSmoother[1] = new TextureGenerator(Window::getWindowWidth(), Window::getWindowHeight(), GL_RGB32F, GL_NONE, GL_NEAREST);

	_fluidThicknessMapSmoother[0] = new TextureGenerator(Window::getWindowWidth(), Window::getWindowHeight(), GL_RGB32F, GL_NONE, GL_NEAREST);
	_fluidThicknessMapSmoother[1] = new TextureGenerator(Window::getWindowWidth(), Window::getWindowHeight(), GL_RGB32F, GL_NONE, GL_NEAREST);
	_fluidNoiseMapSmoother[0] = new TextureGenerator(Window::getWindowWidth(), Window::getWindowHeight(), GL_RGB32F, GL_NONE, GL_NEAREST);
	_fluidNoiseMapSmoother[1] = new TextureGenerator(Window::getWindowWidth(), Window::getWindowHeight(), GL_RGB32F, GL_NONE, GL_NEAREST);

	_noiseShader = new Shader("src/engine/renderer/shaders/fluid/particle.vs", "src/engine/renderer/shaders/fluid/noise.fs");
	_fluidNoiseMapGenerator = new TextureGenerator(Window::getWindowWidth(), Window::getWindowHeight(), GL_RGB32F, GL_NONE, GL_NEAREST);
}

void FluidRenderer::draw(Camera *camera, vector<RenderCommand> *renderCommands)
{
	drawDepth(camera, renderCommands);
	smoothDepth(camera);
	drawThickness(camera, renderCommands);
	smoothThickness(camera);
}

Texture *FluidRenderer::getDepthMap()
{
	return _fluidDepthMapSmoother[1]->getColorTarget();
}

Texture *FluidRenderer::getThicknessMap()
{
	return _fluidThicknessMapSmoother[1]->getColorTarget();
}

Texture *FluidRenderer::getNoiseMap()
{
	return _fluidNoiseMapSmoother[1]->getColorTarget();
}

void FluidRenderer::drawDepth(Camera *camera, vector<RenderCommand> *renderCommands)
{
	_depthShader->use();
	_depthShader->setUniformMat4("proj", camera->getProjectionMatrix());
	_depthShader->setUniformMat4("view", camera->getViewMatrix());
	

	_fluidDepthMapGenerator->bind(true);
	for (RenderCommand renderCommand : *renderCommands)
	{
		Mesh *mesh = renderCommand.mesh;
		GLuint VAO = mesh->getVAO();
		GLuint nIndices = mesh->getNIndices();
		glm::mat4 modelMatrix = renderCommand.modelMatrix;

		_depthShader->setUniform1f("radius", mesh->getMaterial()->getFloat("radius"));
		_depthShader->setUniformMat4("model", modelMatrix);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLE_STRIP, nIndices, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
	_fluidDepthMapGenerator->unbind();
}

void FluidRenderer::smoothDepth(Camera *camera)
{
	GLboolean horizontal = true;
	GLboolean first = true;

	_curvatureFlow->use();
	_curvatureFlow->setUniformMat4("proj", camera->getProjectionMatrix());
	_curvatureFlow->setUniform1f("dt", 0.005f);

	for (GLuint i = 0; i < 25; i++)
	{
		_fluidDepthMapSmoother[horizontal]->bind(true);

		if (first)
		{
			_curvatureFlow->bindTexture(_fluidDepthMapGenerator->getColorTarget(), GL_TEXTURE0, "depth");
			first = false;
		}
		else
		{
			_curvatureFlow->bindTexture(_fluidDepthMapSmoother[!horizontal]->getColorTarget(), GL_TEXTURE0, "depth");
		}

		GLuint VAO = _screenAlignedQuad->getVAO();
		GLuint nIndices = _screenAlignedQuad->getNIndices();

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLE_STRIP, nIndices, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		horizontal = !horizontal;

	}
	_fluidDepthMapSmoother[1]->unbind();
}

void FluidRenderer::drawThickness(Camera *camera, vector<RenderCommand> *renderCommands)
{
	_thicknessShader->use();
	_thicknessShader->setUniformMat4("proj", camera->getProjectionMatrix());
	_thicknessShader->setUniformMat4("view", camera->getViewMatrix());
	_thicknessShader->setUniform1f("radius", 1.0f);

	_fluidThicknessMapGenerator->bind(true);
	glDepthFunc(GL_ALWAYS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	for (RenderCommand renderCommand : *renderCommands)
	{
		Mesh *mesh = renderCommand.mesh;
		GLuint VAO = mesh->getVAO();
		GLuint nIndices = mesh->getNIndices();
		glm::mat4 modelMatrix = renderCommand.modelMatrix;

		_thicknessShader->setUniformMat4("model", modelMatrix);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLE_STRIP, nIndices, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
	glDisable(GL_BLEND);
	glDepthFunc(GL_LESS);
	_fluidThicknessMapGenerator->unbind();
}

void FluidRenderer::smoothThickness(Camera *camera)
{
	GLboolean horizontal = true;
	GLboolean first = true;

	_smoother->use();

	for (GLuint i = 0; i < 10; i++)
	{
		_fluidThicknessMapSmoother[horizontal]->bind(true);

		_smoother->setUniform1i("horizontal", horizontal);

		if (first)
		{
			_smoother->bindTexture(_fluidThicknessMapGenerator->getColorTarget(), GL_TEXTURE1, "image");
			first = false;
		}
		else
		{
			_smoother->bindTexture(_fluidThicknessMapSmoother[!horizontal]->getColorTarget(), GL_TEXTURE1, "image");
		}

		GLuint VAO = _screenAlignedQuad->getVAO();
		GLuint nIndices = _screenAlignedQuad->getNIndices();

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLE_STRIP, nIndices, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		horizontal = !horizontal;

	}
	_fluidThicknessMapSmoother[1]->unbind();
}

void FluidRenderer::drawNoise(Camera *camera, vector<RenderCommand> *renderCommands)
{
	_noiseShader->use();
	_noiseShader->setUniformMat4("proj", camera->getProjectionMatrix());
	_noiseShader->setUniformMat4("view", camera->getViewMatrix());
	_noiseShader->setUniform1f("radius", 0.2f);

	_fluidNoiseMapGenerator->bind(true);
	for (RenderCommand renderCommand : *renderCommands)
	{
		Mesh *mesh = renderCommand.mesh;
		GLuint VAO = mesh->getVAO();
		GLuint nIndices = mesh->getNIndices();
		glm::mat4 modelMatrix = renderCommand.modelMatrix;

		_noiseShader->setUniformMat4("model", modelMatrix);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLE_STRIP, nIndices, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
	_fluidNoiseMapGenerator->unbind();
}

void FluidRenderer::smoothNoise()
{
	GLboolean horizontal = true;
	GLboolean first = true;

	_smoother->use();

	for (GLuint i = 0; i < 10; i++)
	{
		_fluidNoiseMapSmoother[horizontal]->bind(true);

		_smoother->setUniform1i("horizontal", horizontal);

		if (first)
		{
			_smoother->bindTexture(_fluidNoiseMapGenerator->getColorTarget(), GL_TEXTURE0, "image");
			first = false;
		}
		else
		{
			_smoother->bindTexture(_fluidNoiseMapSmoother[!horizontal]->getColorTarget(), GL_TEXTURE0, "image");
		}

		GLuint VAO = _screenAlignedQuad->getVAO();
		GLuint nIndices = _screenAlignedQuad->getNIndices();

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLE_STRIP, nIndices, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		horizontal = !horizontal;

	}
	_fluidNoiseMapSmoother[1]->unbind();
}