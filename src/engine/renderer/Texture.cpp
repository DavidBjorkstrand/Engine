#include "engine/renderer/Texture.h"

#include <string>

#include <SOIL/SOIL.h>
#include <GL/glew.h>

Texture::Texture(string path)
{
	int width;
	int height;
	uint8_t *image;

	image = SOIL_load_image(path.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);

	glGenTextures(1, &_texture);
	bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	SOIL_free_image_data(image);
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