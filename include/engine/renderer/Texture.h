#pragma once

#include <string>

#include <GL/glew.h>

using namespace std;

class Texture
{
	public:
		Texture(string path, GLint wrap, GLint internalFormat, GLenum format, GLenum type);
		Texture();

		GLuint getTextureID();

		void bind();

		void unbind();

	private:
		GLuint _texture;
};