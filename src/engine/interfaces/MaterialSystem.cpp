#include "MaterialSystem.h"
#include "..\renderer\Material.h"
#include "..\MaterialManager.h"

#include <string>

MaterialManager *MaterialSystem::_materialManager = nullptr;

void MaterialSystem::init(MaterialManager *materialManager)
{
	_materialManager = materialManager;
}

Material *MaterialSystem::find(string name)
{
	return _materialManager->find(name);
}