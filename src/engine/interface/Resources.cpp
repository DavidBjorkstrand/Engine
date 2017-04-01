#include "engine/interface/Resources.h"

#include "engine/ResourceSystem.h"
#include "engine/renderer/Material.h"

#include <string>

ResourceSystem *Resources::_resourceSystem = nullptr;

void Resources::init(ResourceSystem *resourceSystem)
{
	_resourceSystem = resourceSystem;
}

Material *Resources::findMaterial(string name)
{
	return _resourceSystem->findMaterial(name);
}

Shader *Resources::findShader(string name)
{
	return _resourceSystem->findShader(name);
}