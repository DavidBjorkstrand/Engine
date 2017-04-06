#pragma once

#include <GL\glew.h>

class Texture;
class Shader;

struct RenderCommand;


class TextureGenerator
{
	public:
		TextureGenerator(GLuint width, GLuint height, GLenum colorFormat, GLenum depthFormat, GLint textureFilter);
		Texture *getColorTarget();
		Texture *getDepthTarget();
		void bind(GLboolean clear);
		void unbind();

	private:
		GLuint _width;
		GLuint _height;
		Texture *_colorTarget;
		Texture *_depthTarget;
		GLuint _FBO;
};