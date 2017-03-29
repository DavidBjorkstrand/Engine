#include "engine/renderer/Material.h"

#include <string>

#include <glm/gtc/type_ptr.hpp>

Material::Material(string name, glm::vec3 albedo, float roughness, float metallic)
{
	_name = name;
	_albedo = albedo;
	_roughness = roughness;
	_metallic = metallic;
}

string Material::getName()
{
	return _name;
}

glm::vec3 Material::getAlbedo()
{
	return _albedo;
}

float Material::getRoughness()
{
	return _roughness;
}

float Material::getMetallic()
{
	return _metallic;
}