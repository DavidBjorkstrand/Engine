#pragma once

#include <map>
#include <string>

#include <GL\glew.h>

using namespace std;

class CubeMap
{
	public:
		CubeMap(map<string, string> *pathMap);

		GLuint getTextureID();

		void bind();
		void unbind();

	private:
		GLenum getTarget(string orientation);

		GLuint _texture;
};