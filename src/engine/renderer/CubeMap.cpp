#include "engine/renderer/CubeMap.h"
#include "engine/renderer/Shader.h"
#include "engine/renderer/Texture.h"
#include "engine/scene/entity/component/Mesh.h"
#include "engine/interface/Resources.h"

#include <string>
#include <iostream>
#include <map>

//#include <SOIL/SOIL.h>
#include <GL\glew.h>
#include "stb_image.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

CubeMap::CubeMap(map<string, string> *pathMap)
{
	int width;
	int height;
	int nrComponents;
	float *image;

	glGenTextures(1, &_texture);
	bind();

	for (map<string, string>::iterator it = pathMap->begin(); it != pathMap->end(); ++it)
	{
		string orientation = it->first;
		string path = it->second;
		GLenum target = getTarget(orientation);

		image = stbi_loadf(path.c_str(), &width, &height, &nrComponents, 0);
		glTexImage2D(target, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, image);

		stbi_image_free(image);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	
	unbind();

	delete pathMap;
}

CubeMap::CubeMap(GLuint size, bool mipmap)
{
	glGenTextures(1, &_texture);
	bind();

	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F, size, size, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (mipmap)
	{
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}

	unbind();
}

CubeMap::CubeMap(string path)
{
	Texture *texture = new Texture(path, GL_CLAMP_TO_EDGE, GL_RGB32F, GL_RGB, GL_FLOAT);

	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	GLuint captureFBO;
	GLuint captureRBO;

	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

	glGenTextures(1, &_texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _texture);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
			512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	Shader *shader = Resources::findShader("equi");
	Mesh *mesh = Mesh::createCube();
	GLuint VAO = mesh->getVAO();
	GLuint nIndices = mesh->getNIndices();

	shader->use();
	shader->bindTexture(texture, GL_TEXTURE0, "equirectangularMap");
	shader->setUniformMat4("proj", captureProjection);

	glViewport(0, 0, 512, 512);
	glDepthFunc(GL_LEQUAL);
	for (GLuint i = 0; i < 6; ++i)
	{
		shader->setUniformMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, _texture, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLE_STRIP, nIndices, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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