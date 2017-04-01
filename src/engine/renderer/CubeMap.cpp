#include "engine/renderer/CubeMap.h"

#include <string>
#include <map>

#include <SOIL/SOIL.h>

#include <GL\glew.h>

CubeMap::CubeMap(map<string, string> *pathMap)
{
	int width;
	int height;
	uint8_t *image;

	glGenTextures(1, &_texture);
	bind();

	for (map<string, string>::iterator it = pathMap->begin(); it != pathMap->end(); ++it)
	{
		string orientation = it->first;
		string path = it->second;
		GLenum target = getTarget(orientation);

		image = SOIL_load_image(path.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(target, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

		SOIL_free_image_data(image);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	
	unbind();

	delete pathMap;
}

GLuint CubeMap::getTextureID()
{
	return _texture;
}

void CubeMap::bind()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, _texture);
}

void CubeMap::unbind()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

GLenum CubeMap::getTarget(string orientation)
{
	if (orientation == "RIGHT")
	{
		return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
	} 
	else if (orientation == "LEFT")
	{
		return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
	}
	else if (orientation == "TOP")
	{
		return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
	}
	else if (orientation == "BOTTOM")
	{
		return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
	}
	else if (orientation == "BACK")
	{
		return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
	}
	else if (orientation == "FRONT")
	{
		return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
	}
}