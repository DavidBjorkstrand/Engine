#include "engine/interface/MaterialSystem.h"

#include "engine/MaterialManager.h"
#include "engine/renderer/Material.h"

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