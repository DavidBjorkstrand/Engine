#include "engine/ResourceSystem.h"

#include "engine/renderer/Material.h"
#include "engine/renderer/Texture.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>

#include "json.hpp"
#include <glm/gtc/type_ptr.hpp>

ResourceSystem::ResourceSystem()
{
	loadMaterials();
}

ResourceSystem::~ResourceSystem()
{
	for (Material *material : *_materials)
	{
		delete material;
	}

	delete _materials;
}

Material *ResourceSystem::findMaterial(string name)
{
	for (Material *material : *ResourceSystem::_materials)
	{
		if (material->getName() == name)
		{
			return material;
		}
	}
}

void ResourceSystem::loadMaterials()
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
			string materialPath = "resources\\materials\\";
			materialPath.append(data.cFileName);

			ifstream file(materialPath);
			nlohmann::json j;

			file >> j;

			glm::vec3 albedo(j["albedo"]["r"], j["albedo"]["g"], j["albedo"]["b"]);
			Material *material = new Material(j["name"], albedo, j["roughness"], j["metallic"]);

			Texture *texture;
			string texturePathBase = "resources\\textures\\";

			if (!j["albedoMap"].is_null())
			{
				string texturePath = texturePathBase;
				string imagePath = j["albedoMap"];
				texturePath.append(imagePath);

				texture = new Texture(texturePath);

				material->setAlbedoMap(texture);
			}

			if (!j["roughnessMap"].is_null())
			{
				string texturePath = texturePathBase;
				string imagePath = j["roughnessMap"];
				texturePath.append(imagePath);

				texture = new Texture(texturePath);

				material->setRoughnessMap(texture);
			}

			if (!j["metallicMap"].is_null())
			{
				string texturePath = texturePathBase;
				string imagePath = j["metallicMap"];
				texturePath.append(imagePath);

				texture = new Texture(texturePath);

				material->setMetallicMap(texture);
			}
			

			_materials->push_back(material);

		} while (FindNextFile(hFind, &data));

		FindClose(hFind);
	}
}
