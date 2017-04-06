#pragma once

#include <string>

#include <GL/glew.h>

using namespace std;

class Texture
{
	public:
		Texture(string path, GLint wrap, GLint internalFormat, GLenum format, GLenum type);
		Texture(GLuint width, GLuint height, GLint internalFormat, GLint wrap, GLint filter);

		GLuint getTextureID();

		void bind();

		void unbind();

	private:
		GLenum internalFormatToFormat(GLint internalFormat);
		GLuint _texture;
};