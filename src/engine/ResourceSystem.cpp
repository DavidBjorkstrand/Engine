#include "engine/ResourceSystem.h"

#include "engine/renderer/Material.h"
#include "engine/renderer/Texture.h"
#include "engine/renderer/Shader.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>

#include "json.hpp"
#include <glm/gtc/type_ptr.hpp>

ResourceSystem::ResourceSystem()
{
	loadShaders();
	loadMaterials();
}

ResourceSystem::~ResourceSystem()
{
	for (map<string, Material*>::iterator it = _materials->begin(); it != _materials->end(); ++it)
	{
		delete it->second;
	}

	delete _materials;

	for (map<string, Shader*>::iterator it = _shaders->begin(); it != _shaders->end(); ++it)
	{
		delete it->second;
	}

	delete _shaders;
}

Material *ResourceSystem::findMaterial(string name)
{
	return _materials->at(name);
}

Shader *ResourceSystem::findShader(string name)
{
	return _shaders->at(name);
}

void ResourceSystem::loadMaterials()
{
	HANDLE hFind;
	WIN32_FIND_DATA data;

	_materials = new map<string, Material*>();

	Material *material = new Material();
	material->setName("default");
	material->setShader("default");
	material->setVec3("uAlbedo", glm::vec3(1.0f, 1.0f, 1.0f));
	material->setFloat("uRoughness", 1.0f);
	material->setFloat("uMetallic", 0.0f);
	_materials->insert(pair<string, Material*>("default", material));

	material = new Material();
	material->setName("skybox");

	hFind = FindFirstFile("resources\\materials\\*.mat", &data);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do {
			string materialPath = "resources\\materials\\";
			materialPath.append(data.cFileName);

			ifstream file(materialPath);
			nlohmann::json j;

			file >> j;

			Material *material = new Material();
			for (nlohmann::json::iterator it = j.begin(); it != j.end(); ++it)
			{
				string key = it.key();

				if (key == "name")
				{
					material->setName(j[key]);
				}
				else if (key == "shader")
				{
					material->setShader(j[key]);
				}
				else
				{
					string type = j[key]["type"];

					if (type == "vec3")
					{
						glm::vec3 value;

						value.x = j[key]["value"]["x"];
						value.y = j[key]["value"]["y"];
						value.z = j[key]["value"]["z"];

						material->setVec3(key, value);
					}
					else if (type == "float")
					{
						float value = j[key]["value"];

						material->setFloat(key, value);
					}
					else if (type == "texture")
					{
						string path = j[key]["value"];
						Texture *texture = new Texture(path, GL_REPEAT, GL_RGB32F, GL_RGB, GL_UNSIGNED_BYTE);

						material->setTexture(key, texture);
					}
				}
			}

			_materials->insert(pair<string, Material*>(material->getName(), material));

		} while (FindNextFile(hFind, &data));

		FindClose(hFind);
	}
}

void ResourceSystem::loadShaders()
{
	_shaders = new map<string, Shader*>();

	map<string, string> *uniforms = new map<string, string>();
	uniforms->insert(pair<string, string>("uAlbedo", "vec3"));
	uniforms->insert(pair<string, string>("uRoughness", "float"));
	uniforms->insert(pair<string, string>("uMetallic", "float"));
	uniforms->insert(pair<string, string>("albedoMap", "texture"));
	uniforms->insert(pair<string, string>("roughnessMap", "texture"));
	uniforms->insert(pair<string, string>("metallicMap", "texture"));
	uniforms->insert(pair<string, string>("normalMap", "texture"));
	uniforms->insert(pair<string, string>("irradianceMap", "samplerCube"));
	uniforms->insert(pair<string, string>("preFilterEnvMap", "samplerCube"));
	uniforms->insert(pair<string, string>("brdfLUT", "texture"));

	Shader *shader = new Shader("src/engine/renderer/shaders/pbr.vs", "src/engine/renderer/shaders/pbr.fs", uniforms);

	_shaders->insert(pair<string, Shader*>("default", shader));

	uniforms = new map<string, string>();
	uniforms->insert(pair<string, string>("skybox", "samplerCube"));

	shader = new Shader("src/engine/renderer/shaders/skybox.vs", "src/engine/renderer/shaders/skybox.fs", uniforms);

	_shaders->insert(pair<string, Shader*>("skybox", shader));

	shader = new Shader("src/engine/renderer/shaders/irradiance.vs", "src/engine/renderer/shaders/irradiance.fs");

	_shaders->insert(pair<string, Shader*>("irradiance", shader));

	shader = new Shader("src/engine/renderer/shaders/irradiance.vs", "src/engine/renderer/shaders/preFilterEnvMap.fs");

	_shaders->insert(pair<string, Shader*>("preFilterEnvMap", shader));

	shader = new Shader("src/engine/renderer/shaders/brdf.vs", "src/engine/renderer/shaders/brdf.fs");

	_shaders->insert(pair<string, Shader*>("brdf", shader));

	shader = new Shader("src/engine/renderer/shaders/equi.vs", "src/engine/renderer/shaders/equi.fs");
	_shaders->insert(pair<string, Shader*>("equi", shader));

	shader = new Shader("src/engine/renderer/shaders/depth.vs", "src/engine/renderer/shaders/depth.fs");
	_shaders->insert(pair<string, Shader *>("depth", shader));

	shader = new Shader("src/engine/renderer/shaders/screenAlignedQuad.vs", "src/engine/renderer/shaders/screenAlignedQuad.fs");
	_shaders->insert(pair<string, Shader *>("screenAlignedQuad", shader));

	shader = new Shader("src/engine/renderer/shaders/screenAlignedQuad.vs", "src/engine/renderer/shaders/particlePbr.fs");
	_shaders->insert(pair<string, Shader *>("particlePbr", shader));
}
