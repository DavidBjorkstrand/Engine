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

Texture::Texture()
{
	glGenTextures(1, &_texture);
	bind();

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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