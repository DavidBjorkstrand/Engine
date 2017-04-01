#include "engine/renderer/Material.h"

#include "engine/renderer/Texture.h"
#include "engine/renderer/Shader.h"
#include "engine/renderer/CubeMap.h"
#include "engine/interface/Resources.h"

#include <string>
#include <map>

#include <glm/gtc/type_ptr.hpp>

Material::Material()
{
	_vec3 = new map<string, glm::vec3>();
	_float = new map<string, float>();
	_texture = new map<string, Texture*>();
	_cubeMap = new map<string, CubeMap*>();
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
	_vec3->insert(pair<string, glm::vec3>(name, vec3));
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

glm::vec3 Material::getVec3(string name)
{
	return _vec3->at(name);
}

float Material::getFloat(string name)
{
	return _float->at(name);
}

Texture *Material::getTexture(string name)
{
	return _texture->at(name);
}

CubeMap *Material::getCubeMap(string name)
{
	return _cubeMap->at(name);
}