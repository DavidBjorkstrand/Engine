#pragma once

#include <string>

#include <GL/glew.h>

using namespace std;

class Texture
{
	public:
		Texture(string path);

		GLuint getTextureID();

		void bind();

		void unbind();

	private:
		GLuint _texture;
};