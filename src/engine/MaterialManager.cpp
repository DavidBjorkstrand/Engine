#include "MaterialManager.h"
#include "renderer\Material.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>

#include "json.hpp"
#include <glm/gtc/type_ptr.hpp>

MaterialManager::MaterialManager()
{
	loadMaterials();
}

MaterialManager::~MaterialManager()
{
	for (Material *material : *_materials)
	{
		delete material;
	}

	delete _materials;
}

Material *MaterialManager::find(string name)
{
	for (Material *material : *MaterialManager::_materials)
	{
		if (material->getName() == name)
		{
			return material;
		}
	}
}

void MaterialManager::loadMaterials()
{
	HANDLE hFind;
	WIN32_FIND_DATA data;

	_materials = new vector<Material *>();

	Material *material = new Material("default", glm::vec3(0.8f), 1.0f, 0.0f);
	_materials->push_back(material);

	hFind = FindFirstFile("resources\\materials\\*.mat", &data);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do {
			string fullPath = "resources\\materials\\";
			fullPath.append(data.cFileName);

			ifstream file(fullPath);
			nlohmann::json j;

			file >> j;

			glm::vec3 albedo(j["albedo"]["r"], j["albedo"]["g"], j["albedo"]["b"]);
			Material *material = new Material(j["name"], albedo, j["roughness"], j["metallic"]);

			MaterialManager::_materials->push_back(material);

		} while (FindNextFile(hFind, &data));

		FindClose(hFind);
	}
}
