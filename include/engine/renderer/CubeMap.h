#pragma once

#include <map>
#include <string>

#include <GL\glew.h>

using namespace std;

class CubeMap
{
	public:
		CubeMap(map<string, string> *pathMap);
		CubeMap(GLuint size, bool mipmap);
		CubeMap(string path);

		GLuint getTextureID();

		void bind();
		void unbind();

	private:
		GLenum getTarget(string orientation);

		GLuint _texture;
};