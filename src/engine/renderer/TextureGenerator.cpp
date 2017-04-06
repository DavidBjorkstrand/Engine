#include "engine/renderer/TextureGenerator.h"

#include "engine/renderer/Texture.h"
#include "engine/renderer/Shader.h"
#include "engine/renderer/RenderSystem.h"
#include "engine/scene/entity/component/Mesh.h"
#include "engine/renderer/Material.h"

#include <GL\glew.h>

TextureGenerator::TextureGenerator(GLuint width, GLuint height, GLenum colorFormat, GLenum depthFormat, GLint textureFilter)
{
	glGenFramebuffers(1, &_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, _FBO);

	if (colorFormat != GL_NONE)
	{
		_colorTarget = new Texture(width, height, colorFormat, GL_CLAMP_TO_EDGE, textureFilter);
		_colorTarget->bind();

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _colorTarget->getTextureID(), 0);
		_colorTarget->unbind();
	}
	else 
	{
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}

	if (depthFormat != GL_NONE)
	{
		_depthTarget = new Texture(width, height, GL_DEPTH_COMPONENT, GL_CLAMP_TO_EDGE, textureFilter);
		_depthTarget->bind();

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthTarget->getTextureID(), 0);
		_depthTarget->unbind();
	}
	else
	{
		GLuint RBO;

		glGenRenderbuffers(1, &RBO);
		glBindRenderbuffer(GL_RENDERBUFFER, RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Texture *TextureGenerator::getColorTarget()
{
	return _colorTarget;
}

Texture *TextureGenerator::getDepthTarget()
{
	return _depthTarget;
}

void TextureGenerator::bind(GLboolean clear)
{
	glBindFramebuffer(GL_FRAMEBUFFER, _FBO);

	if (clear)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}

void TextureGenerator::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}