#include "engine/renderer/Texture.h"

#include <string>
#include <iostream>

//#include <SOIL/SOIL.h>
#include <GL/glew.h>
#include "stb_image.h"

Texture::Texture(string path, GLint wrap, GLint internalFormat, GLenum format, GLenum type)
{
	int width;
	int height;
	int nrComponents;
	float *image;

	stbi_set_flip_vertically_on_load(true);
	image = stbi_loadf(path.c_str(), &width, &height, &nrComponents, 0);

	cout << nrComponents << endl;
	if (image)
	{
		cout << "good" << endl;
	}
	else
	{
		cout << "bad" << endl;
	}

	glGenTextures(1, &_texture);
	bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (nrComponents == 1)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, image);
	}
	else if (nrComponents == 3)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, image);
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, image);
	}
	

	stbi_image_free(image);
	unbind();
}

Texture::Texture(GLuint width, GLuint height, GLint internalFormat, GLint wrap, GLint filter)
{
	glGenTextures(1, &_texture);
	bind();

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, internalFormatToFormat(internalFormat), GL_FLOAT, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

	unbind();
}

GLuint Texture::getTextureID()
{
	return _texture;
}

void Texture::bind()
{
	glBindTexture(GL_TEXTURE_2D, _texture);
}

void Texture::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLenum Texture::internalFormatToFormat(GLint internalFormat)
{
	if ((internalFormat == GL_RED) || (internalFormat == GL_R16F) || (internalFormat == GL_R32F))
	{
		return GL_RED;
	}
	else if ((internalFormat == GL_RG) || (internalFormat == GL_RG16F) || (internalFormat == GL_RG32F))
	{
		return GL_RG;
	}
	else if ((internalFormat == GL_RGB) || (internalFormat == GL_RGB16F) || (internalFormat == GL_RGB32F))
	{
		return GL_RGB;
	}
	else if ((internalFormat == GL_RGBA) || (internalFormat == GL_RGBA16F) || (internalFormat == GL_RGBA32F))
	{
		return GL_RGBA;
	}
	else if ((internalFormat == GL_DEPTH_COMPONENT) || (internalFormat == GL_DEPTH_COMPONENT24) || (internalFormat == GL_DEPTH_COMPONENT32))
	{
		return GL_DEPTH_COMPONENT;
	}
}