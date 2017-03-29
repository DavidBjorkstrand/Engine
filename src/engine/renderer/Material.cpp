#include "engine/renderer/Material.h"

#include "engine/renderer/Texture.h"

#include <string>

#include <glm/gtc/type_ptr.hpp>

Material::Material(string name, glm::vec3 albedo, float roughness, float metallic)
{
	_name = name;
	_albedo = albedo;
	_roughness = roughness;
	_metallic = metallic;
}

Material::~Material()
{
	delete _albedoMap;
	delete _roughnessMap;
	delete _metallicMap;
}

string Material::getName()
{
	return _name;
}

glm::vec3 Material::getAlbedo()
{
	return _albedo;
}

Texture *Material::getAlbedoMap()
{
	return _albedoMap;
}

void Material::setAlbedoMap(Texture *texture)
{
	_albedoMap = texture;
}

float Material::getRoughness()
{
	return _roughness;
}

Texture *Material::getRoughnessMap()
{
	return _roughnessMap;
}

void Material::setRoughnessMap(Texture *texture)
{
	_roughnessMap = texture;
}

float Material::getMetallic()
{
	return _metallic;
}

Texture *Material::getMetallicMap()
{
	return _metallicMap;
}

void Material::setMetallicMap(Texture *texture)
{
	_metallicMap = texture;
}