#include "engine/renderer/Material.h"

#include "engine/renderer/Texture.h"
#include "engine/renderer/Shader.h"
#include "engine/renderer/CubeMap.h"
#include "engine/interface/Resources.h"

#include <string>
#include <map>
#include <stdexcept>

#include <glm/gtc/type_ptr.hpp>
#include <GL\glew.h>

Material::Material()
{
	_vec3 = new map<string, glm::vec3>();
	_float = new map<string, float>();
	_texture = new map<string, Texture*>();
	_cubeMap = new map<string, CubeMap*>();

	_blend = false;
	_blendSrc = GL_SRC_ALPHA;
	_blendDst = GL_ONE_MINUS_SRC_ALPHA;
}

Material::~Material()
{
	delete _vec3;
	delete _float;
	delete _texture;
}

void Material::setName(string name)
{
	_name = name;
}

void Material::setShader(string shaderName)
{
	_shaderName = shaderName;
}

void Material::setVec3(string name, glm::vec3 vec3)
{
	//_vec3->insert(pair<string, glm::vec3>(name, vec3));
	(*_vec3)[name] = vec3;
}

void Material::setFloat(string name, float f)
{
	_float->insert(pair<string, float>(name, f));
}

void Material::setTexture(string name, Texture *texture)
{
	_texture->insert(pair<string, Texture*>(name, texture));
}

void Material::setCubeMap(string name, CubeMap *cubeMap)
{
	_cubeMap->insert(pair<string, CubeMap*>(name, cubeMap));
}

void Material::setBlend(GLboolean blend)
{
	_blend = blend;
}

void Material::setBlendSrc(GLenum blendSrc)
{
	_blendSrc = blendSrc;
}

void Material::setBlendDst(GLenum blendDst)
{
	_blendDst = blendDst;
}

string Material::getName()
{
	return _name;
}

Shader *Material::getShader()
{
	if (_shader == nullptr)
	{
		_shader = Resources::findShader(_shaderName);
	}

	return _shader;
}

string Material::getShaderName()
{
	return _shaderName;
}

glm::vec3 Material::getVec3(string name)
{
	if (_vec3->find(name) != _vec3->end())
	{
		return _vec3->at(name);
	}
	else
	{
		return glm::vec3(0.0f);
	}
}

float Material::getFloat(string name)
{
	if (_float->find(name) != _float->end())
	{
		return _float->at(name);
	}
	else
	{
		return 0.0f;
	}
}

Texture *Material::getTexture(string name)
{
	if (_texture->find(name) != _texture->end())
	{
		return _texture->at(name);
	}
	else
	{
		return nullptr;
	}
}

CubeMap *Material::getCubeMap(string name)
{
	if (_cubeMap->find(name) != _cubeMap->end())
	{
		return _cubeMap->at(name);
	}
	else
	{
		return nullptr;
	}
}

GLboolean Material::getBlend()
{
	return _blend;
}

GLenum Material::getBlendSrc()
{
	return _blendSrc;
}

GLenum Material::getBlendDst()
{
	return _blendDst;
}